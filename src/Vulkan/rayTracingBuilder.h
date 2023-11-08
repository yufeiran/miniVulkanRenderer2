#pragma once
#include "Common/common.h"
#include "Vulkan/device.h"
#include "Vulkan/commandPool.h"
#include "Vulkan/commandBuffer.h"
#include "Vulkan/buffer.h"
#include "queryPool.h"



namespace mini
{

struct AccelKHR
{
	VkAccelerationStructureKHR accel = VK_NULL_HANDLE;
	Buffer* buffer;
};


class RayTracingBuilder
{


public:

struct BlasInput
{
	std::vector<VkAccelerationStructureGeometryKHR> asGeometry;
	std::vector<VkAccelerationStructureBuildRangeInfoKHR> asBuildOffsetInfo;
	VkBuildAccelerationStructureFlagsKHR flags{0};
};


	RayTracingBuilder(Device&device,uint32_t queueIndex);
	~RayTracingBuilder();

	void buildBlas(const std::vector<BlasInput>&          input,
				   VkBuildAccelerationStructureFlagsKHR   flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);

	void buildTlas(const std::vector<VkAccelerationStructureInstanceKHR>&            instances,
		           VkBuildAccelerationStructureFlagsKHR  flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
					bool                                 update = false,
					bool                                 motion = false);

	VkDeviceAddress getBlasDeviceAddress(uint32_t blasId);

	VkAccelerationStructureKHR getAccelerationStructure() const;
private:


	struct BuildAccelerationStructure
	{
		VkAccelerationStructureBuildGeometryInfoKHR       buildInfo{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR};
		VkAccelerationStructureBuildSizesInfoKHR          sizeInfo {VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR};
		const VkAccelerationStructureBuildRangeInfoKHR*   rangeInfo;
		AccelKHR                                          as;
		AccelKHR                                          cleanupAS;
	};

	void cmdCreateBlas(CommandBuffer& cmdBuf,std::vector<uint32_t> indices,
						std::vector<BuildAccelerationStructure>& buildAs,
						VkDeviceAddress scratchAddress,
						std::unique_ptr<QueryPool> &queryPool);

	void cmdCompactBlas(CommandBuffer& cmdBuf,std::vector<uint32_t> indices,
						std::vector<BuildAccelerationStructure>&    buildAs,
						std::unique_ptr<QueryPool>& queryPool);


	AccelKHR createAcceleration(VkAccelerationStructureCreateInfoKHR& accel);

	void destroyNonCompacted(std::vector<uint32_t> indices,std::vector<BuildAccelerationStructure>& buildAs);

	void cmdCreateTlas(CommandBuffer&                        cmdBuf,
					   uint32_t                              countInstance,
		               VkDeviceAddress                       instBufferAddr,
					   VkBuildAccelerationStructureFlagsKHR  flags,
					   bool                                  update,
		               bool                                  motion
	);
	



	Device& device;
	uint32_t queueIndex{0};

	CommandPool commandPool;

	std::unique_ptr<Buffer> scratchBuffer;
	std::unique_ptr<Buffer> instancesBuffer;

	std::vector<AccelKHR> blasVec;

	std::unique_ptr<CommandBuffer> cmdBuf;

	AccelKHR tlas;
};


inline VkTransformMatrixKHR toTransformMatrixKHR(glm::mat4 matrix)
{
	glm::mat4                temp = glm::transpose(matrix);
	VkTransformMatrixKHR     outMatrix;
	memcpy(&outMatrix,&temp,sizeof(VkTransformMatrixKHR));
	return outMatrix;
}

}
