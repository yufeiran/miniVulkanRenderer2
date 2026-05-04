#include<Rendering/rayTracingPipelineBuilder.h>

namespace mini
{

	auto RayTracingPipelineBuilder::objModelToVkGeometryKHR(const ObjModel& model)
	{
		RayTracingBuilder::BlasInput input;


		VkDeviceAddress vertexAddresss = model.vertexBuffer->getBufferDeviceAddress();
		VkDeviceAddress indexAddress = model.indexBuffer->getBufferDeviceAddress();

		uint32_t maxPrimitiveCount = model.nbIndices / 3;

		VkAccelerationStructureGeometryTrianglesDataKHR triangles{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR };
		triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		triangles.vertexData.deviceAddress = vertexAddresss;
		triangles.vertexStride = sizeof(Vertex);

		triangles.indexType = VK_INDEX_TYPE_UINT32;
		triangles.indexData.deviceAddress = indexAddress;

		triangles.maxVertex = model.nbVertices - 1;

		VkAccelerationStructureGeometryKHR asGeom{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR };
		asGeom.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		asGeom.flags = VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;  //Avoid double hits
		asGeom.geometry.triangles = triangles;

		VkAccelerationStructureBuildRangeInfoKHR offset = {};

		offset.firstVertex = 0;
		offset.primitiveCount = maxPrimitiveCount;
		offset.primitiveOffset = 0;
		offset.transformOffset = 0;


		input.asGeometry.emplace_back(asGeom);
		input.asBuildOffsetInfo.emplace_back(offset);

		return input;

	}

	void RayTracingPipelineBuilder::initRayTracingRender(Device& device, PhysicalDevice& phyDevice)
	{
		if (!rayTracingBuilder)
		{
			rayTracingBuilder.reset();
		}
		// Requesting ray tracing properties
		VkPhysicalDeviceProperties2 prop2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
		prop2.pNext = &rtProperties;
		vkGetPhysicalDeviceProperties2(phyDevice.getHandle(), &prop2);
		rayTracingBuilder = std::make_unique<RayTracingBuilder>(device, device.getGraphicQueue().getIndex());
	}

	void RayTracingPipelineBuilder::buildRayTracing(Device& device, PhysicalDevice& phyDevice, ResourceManager& rm, 
		RenderTarget& offscreenRenderTarget, DescriptorSetLayout& graphicsDescriptorSetLayout)
	{
		initRayTracingRender(device, phyDevice);
		buildAS(rm);
		createRtDescriptorSet(device, offscreenRenderTarget);
		createRtPipeline(device,graphicsDescriptorSetLayout);
		createRtShaderBindingTable(device);
	}
	void RayTracingPipelineBuilder::buildAS(ResourceManager& rm)
	{
		tlas.clear();

		LogTimerStart("build AS");
		createBottomLevelAS(rm);
		createTopLevelAS(rm);
		LogTimerEnd("build AS");
	}

	void RayTracingPipelineBuilder::createBottomLevelAS(ResourceManager& rm)
	{
		std::vector<RayTracingBuilder::BlasInput> allBlas;
		//allBlas.reserve(resourceManagement->getModelSum());

		const auto& models = rm.getModels();

		for (const auto& model : models)
		{
			auto blas = objModelToVkGeometryKHR(*model);

			allBlas.push_back(blas);

		}

		rayTracingBuilder->buildBlas(allBlas, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
	}

	void RayTracingPipelineBuilder::createTopLevelAS(ResourceManager& rm)
	{
		//Log("start createTLAS");


		const auto& instances = rm.getInstances();
		tlas.clear();
		tlas.reserve(instances.size());

		for (const ObjInstance& instance : instances)
		{
			VkAccelerationStructureInstanceKHR rayInst{};
			uint32_t modelId = instance.objIndex;

			GltfShadeMaterial& mat = rm.materials[modelId];

			rayInst.transform = toTransformMatrixKHR(instance.transform);
			rayInst.instanceCustomIndex = modelId;
			rayInst.accelerationStructureReference = rayTracingBuilder->getBlasDeviceAddress(modelId);

			VkGeometryInstanceFlagsKHR flags{};
			if (mat.doubleSided == 1)
			{
				flags |= VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
			}

			rayInst.flags = flags;
			// Only be hit if rayMask & instance.mask != 0
			// Light Mask = 0x02 other = 0x01
			if (instance.type == INSTANCE_TYPE_NORMAL)
			{
				rayInst.mask = 0x01;
			}
			else if (instance.type == INSTANCE_TYPE_LIGHT_CUBE)
			{
				rayInst.mask = 0x02;
			}
			else
			{
				rayInst.mask = 0x01;
			}

			rayInst.instanceShaderBindingTableRecordOffset = 0;
			tlas.emplace_back(rayInst);
		}
		rtFlags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;

		rayTracingBuilder->buildTlas(tlas, rtFlags);
	}

	void RayTracingPipelineBuilder::createRtDescriptorSet(Device& device, RenderTarget& offscreenRenderTarget)
	{
		rtDescSetBindings.addBinding(RtBindings::eTlas, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1,
			VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR); // TLAS
		rtDescSetBindings.addBinding(RtBindings::eOutImage, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1,
			VK_SHADER_STAGE_RAYGEN_BIT_KHR);   // Output image

		rtDescPool = rtDescSetBindings.createPool(device);
		rtDescSetLayout = rtDescSetBindings.createLayout(device);

		auto descSetLayout = rtDescSetLayout->getHandle();

		VkDescriptorSetAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocateInfo.descriptorPool = rtDescPool->getHandle();
		allocateInfo.descriptorSetCount = 1;
		allocateInfo.pSetLayouts = &descSetLayout;
		vkAllocateDescriptorSets(device.getHandle(), &allocateInfo, &rtDescSet);

		VkAccelerationStructureKHR                         tlas = rayTracingBuilder->getAccelerationStructure();
		VkWriteDescriptorSetAccelerationStructureKHR       descASInfo{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR };
		descASInfo.accelerationStructureCount = 1;
		descASInfo.pAccelerationStructures = &tlas;
		VkDescriptorImageInfo imageInfo{ {},offscreenRenderTarget.getImageViewByIndex(0).getHandle(),VK_IMAGE_LAYOUT_GENERAL };

		std::vector<VkWriteDescriptorSet> writes;
		writes.emplace_back(rtDescSetBindings.makeWrite(rtDescSet, RtBindings::eTlas, &descASInfo));
		writes.emplace_back(rtDescSetBindings.makeWrite(rtDescSet, RtBindings::eOutImage, &imageInfo));

		vkUpdateDescriptorSets(device.getHandle(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
	}

	void RayTracingPipelineBuilder::updateRtDescriptorSet(Device& device, RenderTarget& offscreenRenderTarget, Sampler& postRenderImageSampler)
	{
		VkAccelerationStructureKHR                         tlas = rayTracingBuilder->getAccelerationStructure();
		VkWriteDescriptorSetAccelerationStructureKHR       descASInfo{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR };
		descASInfo.accelerationStructureCount = 1;
		descASInfo.pAccelerationStructures = &tlas;
		//VkDescriptorImageInfo imageInfo{ {},offscreenRenderTarget->getImageViewByIndex(0).getHandle(),VK_IMAGE_LAYOUT_GENERAL };



		// update output buffer
		const auto& offscreenColorImageView = offscreenRenderTarget.getImageViewByIndex(0);
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo.imageView = offscreenColorImageView.getHandle();
		imageInfo.sampler = postRenderImageSampler.getHandle();
		VkWriteDescriptorSet writeDescriptorSets = rtDescSetBindings.makeWrite(rtDescSet, RtBindings::eOutImage, &imageInfo);
		vkUpdateDescriptorSets(device.getHandle(), 1, &writeDescriptorSets, 0, nullptr);

		std::vector<VkWriteDescriptorSet> writes;
		writes.emplace_back(rtDescSetBindings.makeWrite(rtDescSet, RtBindings::eTlas, &descASInfo));
		writes.emplace_back(rtDescSetBindings.makeWrite(rtDescSet, RtBindings::eOutImage, &imageInfo));

		vkUpdateDescriptorSets(device.getHandle(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);

	}


	void RayTracingPipelineBuilder::updateInstances(ResourceManager& rm)
	{
		static auto start = std::chrono::system_clock::now();
		static std::vector<VkAccelerationStructureInstanceKHR> oldTlas = tlas;

		//int lightId =resourceManager->getInstanceId("LightCube");
		auto& instances = rm.getInstances();
		//if(lightId!= -1)
		//{
		//	

		//	auto& lightInstance = instances[lightId];

		//	auto& light = lights[0];

		//	lightInstance.transform = glm::translate(glm::mat4(1.0f),light.getPosition());
		//}

		auto now = std::chrono::system_clock::now();
		std::chrono::duration<float> diff = now - start;
		start = now;


		for (int i = 0; i < instances.size(); i++)
		{
			auto& inst = instances[i];
			VkAccelerationStructureInstanceKHR& tinst = tlas[i];
			tinst.transform = toTransformMatrixKHR(inst.transform);
		}

		bool needUpdate = false;
		if (tlas.size() != oldTlas.size())
		{
			needUpdate = true;
		}
		else {
			for (int i = 0; i < tlas.size(); i++)
			{
				if (memcmp(&tlas[i], &oldTlas[i], sizeof(tlas[i])) != 0)
				{
					needUpdate = true;
					break;
				}
			}
		}
		if (needUpdate == true)
		{
			rayTracingBuilder->buildTlas(tlas, rtFlags, true);
		}

		oldTlas = tlas;

	}

	void RayTracingPipelineBuilder::updateFrame(Camera&camera, PushConstantRay& pcRay,VkExtent2D windowsSize)
	{
		static glm::mat4 refCamMat;
		static float  refFov = camera.getFov();

		const auto& m = camera.getViewMat();
		const auto fov = camera.getFov();

		if (memcmp(&refCamMat[0][0], &m[0][0], sizeof(glm::mat4)) != 0 || refFov != fov)
		{
			resetFrame(pcRay);
			refCamMat = m;
			refFov = fov;
		}
		pcRay.frame++;
		pcRay.totalFrameCount = frameCount;

		float fovY_radians = glm::radians(camera.getFov());
		float pixelSpreadAngle = 2.0f * tan(fovY_radians / 2.0f) / float(windowsSize.height);
		pcRay.pixelSpreadAngle = pixelSpreadAngle;
	}

	void RayTracingPipelineBuilder::resetFrame(PushConstantRay& pcRay)
	{
		pcRay.frame = -1;
	}

	void RayTracingPipelineBuilder::createRtPipeline(Device& device, DescriptorSetLayout& graphicsDescriptorSetLayout)
	{
		enum StageIndices
		{
			eRaygen,
			eMiss0,
			eMiss1,
			eClosetHit,
			eAnyHit,
			eShaderGroupCount
		};


		ShaderModule rayGenShader(getSPVPath("raytrace.rgen.spv"), device, VK_SHADER_STAGE_RAYGEN_BIT_KHR);
		ShaderModule rayCHitShader(getSPVPath("raytrace.rchit.spv"), device, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
		ShaderModule rayAnyHitShader(getSPVPath("raytrace.rahit.spv"), device, VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
		ShaderModule rayMissShader(getSPVPath("raytrace.rmiss.spv"), device, VK_SHADER_STAGE_MISS_BIT_KHR);
		ShaderModule rayShadowMissShader(getSPVPath("raytraceShadow.rmiss.spv"), device, VK_SHADER_STAGE_MISS_BIT_KHR);


		// All stages 
		std::vector<VkPipelineShaderStageCreateInfo> stages{};
		stages.resize(eShaderGroupCount);

		VkPipelineShaderStageCreateInfo stage{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		stage.pName = "main";  // All the same entry point
		// Raygen
		stage.module = rayGenShader.getHandle();
		stage.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		stages[eRaygen] = stage;
		// Miss0 
		stage.module = rayMissShader.getHandle();
		stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
		stages[eMiss0] = stage;
		// Miss1 shadow pass 
		stage.module = rayShadowMissShader.getHandle();
		stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
		stages[eMiss1] = stage;
		// Hit Group - Closest Hit
		stage.module = rayCHitShader.getHandle();
		stage.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		stages[eClosetHit] = stage;
		// Hit Group - Any Hit
		stage.module = rayAnyHitShader.getHandle();
		stage.stage = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		stages[eAnyHit] = stage;

		// Shader groups
		VkRayTracingShaderGroupCreateInfoKHR group{ VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR };
		group.anyHitShader = VK_SHADER_UNUSED_KHR;
		group.closestHitShader = VK_SHADER_UNUSED_KHR;
		group.generalShader = VK_SHADER_UNUSED_KHR;
		group.intersectionShader = VK_SHADER_UNUSED_KHR;

		// Raygen
		group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		group.generalShader = eRaygen;
		rtShaderGroups.push_back(group);

		// Miss0 
		group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		group.generalShader = eMiss0;
		rtShaderGroups.push_back(group);

		// Miss1 shadow
		group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		group.generalShader = eMiss1;
		rtShaderGroups.push_back(group);

		// closet hit shader
		group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
		group.generalShader = VK_SHADER_UNUSED_KHR;
		group.closestHitShader = eClosetHit;
		group.anyHitShader = eAnyHit;
		rtShaderGroups.push_back(group);

		// Push constant
		VkPushConstantRange pushConstant{ VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR ,
										 0, sizeof(PushConstantRay) };
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;

		// Descriptor sets: set 0 for raytracing , set 1 for global scene
		std::vector<VkDescriptorSetLayout> rtDescSetLayouts = { rtDescSetLayout->getHandle(),graphicsDescriptorSetLayout.getHandle() };
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t> (rtDescSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = rtDescSetLayouts.data();

		rtPipelineLayout = std::make_unique<PipelineLayout>(device, pipelineLayoutCreateInfo);

		// Assemble the shader stages and recursion depth info into the ray tracing pipeline
		VkRayTracingPipelineCreateInfoKHR rayPipelineInfo{ VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR };
		rayPipelineInfo.stageCount = static_cast<uint32_t>(stages.size());  // Stages are shaders
		rayPipelineInfo.pStages = stages.data();

		rayPipelineInfo.groupCount = static_cast<uint32_t>(rtShaderGroups.size());
		rayPipelineInfo.pGroups = rtShaderGroups.data();

		rayPipelineInfo.maxPipelineRayRecursionDepth = 31; // Ray depth
		rayPipelineInfo.layout = rtPipelineLayout->getHandle();

		rtPipeline = std::make_unique<RayTracingPipeline>(device, rayPipelineInfo);
	}
	void RayTracingPipelineBuilder::createRtShaderBindingTable(Device& device)
	{
		uint32_t missCount{ 2 };
		uint32_t hitCount{ 1 };
		auto     handleCount = 1 + missCount + hitCount;
		uint32_t handleSize = rtProperties.shaderGroupHandleSize;

		uint32_t handleSizeAligned = align_up(handleSize, rtProperties.shaderGroupHandleAlignment);

		rgenRegion.stride = align_up(handleSizeAligned, rtProperties.shaderGroupBaseAlignment); //step size
		rgenRegion.size = rgenRegion.stride;
		missRegion.stride = handleSizeAligned;
		missRegion.size = align_up(missCount * handleSizeAligned, rtProperties.shaderGroupBaseAlignment);
		hitRegion.stride = handleSizeAligned;
		hitRegion.size = align_up(hitCount * handleSizeAligned, rtProperties.shaderGroupBaseAlignment);

		// Get the shader group handles 
		uint32_t             dataSize = handleCount * handleSize;
		std::vector<uint8_t> handles(dataSize);
		auto result = vkGetRayTracingShaderGroupHandlesKHR(device.getHandle(), rtPipeline->getHandle(), 0, handleCount, dataSize, handles.data());
		assert(result == VK_SUCCESS);

		// Allocate a buffer for storing the SBT
		VkDeviceSize sbtSize = rgenRegion.size + missRegion.size + hitRegion.size + callRegion.size;
		rtSBTBuffer = std::make_unique<Buffer>(device,
			sbtSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
			| VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		rtSBTBuffer->setName("RtSBTBuffer");

		// Find the SBT address of each group
		VkBufferDeviceAddressInfo info{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, nullptr, rtSBTBuffer->getHandle() };
		VkDeviceAddress           sbtAddress = rtSBTBuffer->getBufferDeviceAddress();
		rgenRegion.deviceAddress = sbtAddress;
		missRegion.deviceAddress = sbtAddress + rgenRegion.size;
		hitRegion.deviceAddress = sbtAddress + rgenRegion.size + missRegion.size;

		// Helper to retrieve the handle data
		auto getHandle = [&](int i) { return handles.data() + i * handleSize; };

		rtSBTBuffer->persistentMap(sbtSize);
		// Map the SBT buffer and write in the handles.
		auto* pSBTBuffer = reinterpret_cast<uint8_t*>(rtSBTBuffer->getMapAddress());
		uint8_t* pData{ nullptr };
		uint32_t handleIdx{ 0 };

		// Raygen
		pData = pSBTBuffer;
		memcpy(pData, getHandle(handleIdx++), handleSize);

		// Miss
		pData = pSBTBuffer + rgenRegion.size;
		for (uint32_t c = 0; c < missCount; c++)
		{
			memcpy(pData, getHandle(handleIdx++), handleSize);
			pData += missRegion.stride;
		}

		// Hit 
		pData = pSBTBuffer + rgenRegion.size + missRegion.size;
		for (uint32_t c = 0; c < hitCount; c++)
		{
			memcpy(pData, getHandle(handleIdx++), handleSize);
		}
		rtSBTBuffer->unpersistentMap();
	}

	void RayTracingPipelineBuilder::raytrace(CommandBuffer& cmd, const glm::vec4& clearColor, PushConstantRay& pcRay,const VkDescriptorSet& graphicsDescriptorSet,const VkExtent2D& extent)
	{

		// Initializing push constant values
		pcRay.clearColor = clearColor;


		std::vector<VkDescriptorSet> descSets{ rtDescSet,graphicsDescriptorSet };
		vkCmdBindPipeline(cmd.getHandle(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline->getHandle());
		vkCmdBindDescriptorSets(cmd.getHandle(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipelineLayout->getHandle(), 0,
			(uint32_t)descSets.size(), descSets.data(), 0, nullptr);
		vkCmdPushConstants(cmd.getHandle(), rtPipelineLayout->getHandle(),
			VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR,
			0, sizeof(PushConstantRay), &pcRay);

		vkCmdTraceRaysKHR(cmd.getHandle(), &rgenRegion, &missRegion, &hitRegion, &callRegion, extent.width, extent.height, 1);
	}

}