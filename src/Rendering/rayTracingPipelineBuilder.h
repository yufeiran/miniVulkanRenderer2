#pragma once



#include <Common/common.h>
#include "Common/camera.h"
#include "Rendering/rayTracingBuilder.h"
#include "Vulkan/rayTracingPipeline.h"
#include "Vulkan/descriptorSetBindings.h"
#include <Rendering/rayTracingBuilder.h>
#include "Rendering/renderTarget.h"
#include <ResourceManagement/ResourceManager.h>
#include "Vulkan/shaderModule.h"
#include "Vulkan/graphicsPipeline.h"
#include "Vulkan/device.h"
#include "Vulkan/physicalDevice.h"



namespace mini
{

	class RayTracingPipelineBuilder
	{
	public:
		auto objModelToVkGeometryKHR(const ObjModel& model);

		void initRayTracingRender(Device& device, PhysicalDevice& phyDevice);
		void buildRayTracing(Device& device, PhysicalDevice& phyDevice, ResourceManager& rm, 
			RenderTarget& offscreenRenderTarget, DescriptorSetLayout& graphicsDescriptorSetLayout);
		void buildAS(ResourceManager& rm);
		void createBottomLevelAS(ResourceManager& rm);
		void createTopLevelAS(ResourceManager& rm);
		void createRtDescriptorSet(Device& device, RenderTarget& offscreenRenderTarget);
		void updateRtDescriptorSet(Device& device, RenderTarget& offscreenRenderTarget, Sampler& postRenderImageSampler);

		void updateInstances(ResourceManager& rm);
		void updateFrame(Camera& camera, PushConstantRay& pcRay, VkExtent2D windowsSize);
		void resetFrame(PushConstantRay& pcRay);

		// pram2 is graphicsPipeline descriptor set layout, share some common bindings with rasterization pipeline
		void createRtPipeline(Device& device, DescriptorSetLayout& graphicsDescriptorSetLayout);
		void createRtShaderBindingTable(Device& device);

		void raytrace(CommandBuffer& cmd, const glm::vec4& clearColor, PushConstantRay& pcRay,const VkDescriptorSet& graphicsDescriptorSet,const VkExtent2D& extent);

	
		// Raytracing pipeline data
		VkPhysicalDeviceRayTracingPipelinePropertiesKHR    rtProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR };
		std::unique_ptr<RayTracingBuilder>                 rayTracingBuilder;
		DescriptorSetBindings                              rtDescSetBindings;
		std::vector<std::unique_ptr<ShaderModule>>         rtShaderModules;
		std::unique_ptr<DescriptorPool>                    rtDescPool;
		std::unique_ptr<DescriptorSetLayout>               rtDescSetLayout;
		VkDescriptorSet                                    rtDescSet;
		std::vector<VkRayTracingShaderGroupCreateInfoKHR>  rtShaderGroups;
		std::unique_ptr<PipelineLayout>                    rtPipelineLayout;
		std::unique_ptr<RayTracingPipeline>                rtPipeline;

		std::unique_ptr<Buffer>                            rtSBTBuffer;
		VkStridedDeviceAddressRegionKHR                    rgenRegion{};
		VkStridedDeviceAddressRegionKHR                    missRegion{};
		VkStridedDeviceAddressRegionKHR                    hitRegion{};
		VkStridedDeviceAddressRegionKHR                    callRegion{};
		std::vector<VkAccelerationStructureInstanceKHR>    tlas;
		VkBuildAccelerationStructureFlagsKHR               rtFlags;

	};
}