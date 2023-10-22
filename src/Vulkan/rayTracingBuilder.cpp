#include "Vulkan/rayTracingBuilder.h"
#include <numeric>


using namespace mini;


RayTracingBuilder::RayTracingBuilder(Device& device, uint32_t queueIndex)
	:device(device),queueIndex(queueIndex),commandPool(device)
{

}

RayTracingBuilder::~RayTracingBuilder()
{
	for(auto &accel:blasVec)
	{
		if(accel.accel!=VK_NULL_HANDLE)
		{
			vkDestroyAccelerationStructureKHR(device.getHandle(),accel.accel,nullptr);
		}
		if(accel.buffer!=nullptr)
		{
			accel.buffer->~Buffer();
		}
	}
}

void RayTracingBuilder::buildBlas(const std::vector<BlasInput>& input, VkBuildAccelerationStructureFlagsKHR flags)
{
	Log("start buildBlas");
	auto         nbBlas = static_cast<uint32_t>(input.size());
	VkDeviceSize asTotalSize{0};    
	uint32_t     nbCompactions{0};
	VkDeviceSize maxScratchSize{0};  // Largest scratch size

	std::vector<BuildAccelerationStructure> buildAs(nbBlas);
	for(uint32_t idx = 0; idx < nbBlas; idx++)
	{
		buildAs[idx].buildInfo.type             = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		buildAs[idx].buildInfo.mode             = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		buildAs[idx].buildInfo.flags            = input[idx].flags | flags;
		buildAs[idx].buildInfo.geometryCount    = static_cast<uint32_t>(input[idx].asGeometry.size());
		buildAs[idx].buildInfo.pGeometries      = input[idx].asGeometry.data();

		buildAs[idx].rangeInfo = input[idx].asBuildOffsetInfo.data();


		// Finding sizes to create acceleration structures and scratch
		std::vector<uint32_t> maxPrimCount(input[idx].asBuildOffsetInfo.size());
		for(auto tt = 0; tt < input[idx].asBuildOffsetInfo.size();tt++)
		{
			maxPrimCount[tt] = input[idx].asBuildOffsetInfo[tt].primitiveCount;
		}
		vkGetAccelerationStructureBuildSizesKHR(device.getHandle(),VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
			&buildAs[idx].buildInfo,maxPrimCount.data(),&buildAs[idx].sizeInfo);


		asTotalSize += buildAs[idx].sizeInfo.accelerationStructureSize;
		maxScratchSize = std::max(maxScratchSize,buildAs[idx].sizeInfo.buildScratchSize);
		nbCompactions += hasFlag(buildAs[idx].buildInfo.flags,
			VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR);


	}

	scratchBuffer=std::make_unique<Buffer>(device,maxScratchSize,
		VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT|VK_BUFFER_USAGE_STORAGE_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkDeviceAddress           scratchAddress = scratchBuffer->getBufferDeviceAddress();

	// Allocate a query pool for storing the real need size for every BLAS compaction.
	std::unique_ptr<QueryPool> queryPool;
	if(nbCompactions>0) // need compaction?
	{
		assert(nbCompactions == nbBlas);
		VkQueryPoolCreateInfo qpci{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
		qpci.queryCount = nbBlas;
		qpci.queryType = VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR;
		queryPool = std::make_unique<QueryPool>(device,qpci);
	}
	

	// Create BLAS!
	std::vector<uint32_t> indices; // Indices of the BLAS to create 
	VkDeviceSize          batchSize{0};
	VkDeviceSize          batchLimit{256'000'000}; // 256 MB
	for(uint32_t idx = 0; idx < nbBlas; idx++)
	{
		indices.push_back(idx);
		batchSize+=buildAs[idx].sizeInfo.accelerationStructureSize;

		if(batchSize >= batchLimit || idx == nbBlas - 1)
		{
			std::unique_ptr<CommandBuffer> cmdBuf = commandPool.createCommandBuffer();
			cmdBuf->beginSingleTime();
			cmdCreateBlas(*cmdBuf,indices,buildAs,scratchAddress,queryPool);
			cmdBuf->endSingleTime(device.getGraphicQueue());

			if(queryPool)
			{
				std::unique_ptr<CommandBuffer> cmdBuf = commandPool.createCommandBuffer();
				cmdBuf->beginSingleTime();
				cmdCompactBlas(*cmdBuf,indices,buildAs,queryPool);
				cmdBuf->endSingleTime(device.getGraphicQueue());

				destroyNonCompacted(indices,buildAs);
			}
			
			batchSize = 0;
			indices.clear();
		}
		
	}

	// Logging reduction
	if(queryPool)
	{
		VkDeviceSize compactSize = std::accumulate(buildAs.begin(),buildAs.end(),0ULL,[](const auto& a,const auto& b){
				return a + b.sizeInfo.accelerationStructureSize;
			});

		const float fractionSmaller = (asTotalSize == 0) ? 0 : (asTotalSize - compactSize) / float(asTotalSize);
		Log("BLAS: reducing from "+std::to_string(asTotalSize)+" to "+ std::to_string(compactSize)+ " = "+
			std::to_string(asTotalSize-compactSize) +" ( "+std::to_string(fractionSmaller *100.0f)+"%)" );

	}

	for(auto& b:buildAs)
	{
		blasVec.push_back(b.as);	
	}
}

void RayTracingBuilder::cmdCreateBlas(CommandBuffer& cmdBuf,std::vector<uint32_t> indices,
						std::vector<BuildAccelerationStructure>& buildAs,
						VkDeviceAddress scratchAddress,
						std::unique_ptr<QueryPool> &queryPool)
{
	if(queryPool)
	{
		vkResetQueryPool(device.getHandle(),queryPool->getHandle(),0,static_cast<uint32_t>(indices.size()));
	}
	uint32_t queryCnt{0};

	for(const auto& idx : indices)
	{

		// create BLAS
		VkAccelerationStructureCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR};
		createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		createInfo.size = buildAs[idx].sizeInfo.accelerationStructureSize; // use to allocate memory;
		buildAs[idx].as = createAcceleration(createInfo);

		// BuildInfo more!
		buildAs[idx].buildInfo.dstAccelerationStructure   = buildAs[idx].as.accel;  // Setting where the build lands
		buildAs[idx].buildInfo.scratchData.deviceAddress  = scratchAddress;         // use same scracth buffer to build every BLAS

		// build BLAS!
		vkCmdBuildAccelerationStructuresKHR(cmdBuf.getHandle(),1,&buildAs[idx].buildInfo,&buildAs[idx].rangeInfo);


		// scratch buffer is reused ! need a barrier to ensure one build is finished before staring the next one.
		VkMemoryBarrier barrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
		barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
		barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
		vkCmdPipelineBarrier(cmdBuf.getHandle(), VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, 0, 1, &barrier, 0, nullptr, 0, nullptr);

		if(queryPool)
		{
			// find 'real' amount of memory needed, use for compaction
			vkCmdWriteAccelerationStructuresPropertiesKHR(cmdBuf.getHandle(),1,&buildAs[idx].buildInfo.dstAccelerationStructure,
				VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR,queryPool->getHandle(),queryCnt++);
		}
	}
}

void mini::RayTracingBuilder::cmdCompactBlas(CommandBuffer& cmdBuf,
									std::vector<uint32_t> indices, 
				std::vector<BuildAccelerationStructure>& buildAs, 
							std::unique_ptr<QueryPool>& queryPool)
{
	uint32_t queryCnt{0};

	// Get the compacted size result back
	std::vector<VkDeviceSize> compactSizes(static_cast<uint32_t>(indices.size()));
	vkGetQueryPoolResults(device.getHandle(),queryPool->getHandle(),0,(uint32_t)compactSizes.size(),compactSizes.size()*sizeof(VkDeviceSize),
						compactSizes.data(),sizeof(VkDeviceSize),VK_QUERY_RESULT_WAIT_BIT);

	for(auto idx : indices)
	{
		buildAs[idx].cleanupAS                           = buildAs[idx].as;
		buildAs[idx].sizeInfo.accelerationStructureSize  = compactSizes[queryCnt++];

		// Creating a compact version of the AS 
		VkAccelerationStructureCreateInfoKHR asCreateInfo{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR};
		asCreateInfo.size = buildAs[idx].sizeInfo.accelerationStructureSize;
		asCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		buildAs[idx].as   = createAcceleration(asCreateInfo);

		// Copy old BLAS to new buffer
		VkCopyAccelerationStructureInfoKHR copyInfo{VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR};
		copyInfo.src    = buildAs[idx].buildInfo.dstAccelerationStructure;
		copyInfo.dst    = buildAs[idx].as.accel;
		copyInfo.mode   = VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR;
		vkCmdCopyAccelerationStructureKHR(cmdBuf.getHandle(),&copyInfo);

	}
}

AccelKHR RayTracingBuilder::createAcceleration(VkAccelerationStructureCreateInfoKHR& accel)
{
	AccelKHR resultAccel;
	
	resultAccel.buffer = new Buffer(device,accel.size,VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR|
						VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	accel.buffer = resultAccel.buffer->getHandle();

	VK_CHECK(vkCreateAccelerationStructureKHR(device.getHandle(),&accel,nullptr,&resultAccel.accel));

	return resultAccel;
}

void RayTracingBuilder::destroyNonCompacted(std::vector<uint32_t> indices, std::vector<BuildAccelerationStructure>& buildAs)
{
	for(auto& i : indices)
	{
		auto& cleanupAs = buildAs[i].cleanupAS;
		vkDestroyAccelerationStructureKHR(device.getHandle(),cleanupAs.accel,nullptr);
		cleanupAs.buffer->~Buffer();
		cleanupAs.buffer = nullptr;
	}
}
