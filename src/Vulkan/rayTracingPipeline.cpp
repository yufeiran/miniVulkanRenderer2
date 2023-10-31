#include "Vulkan/rayTracingPipeline.h"

using namespace mini;

RayTracingPipeline::RayTracingPipeline(std::vector<VkRayTracingShaderGroupCreateInfoKHR>& shaderGroup,
								       PipelineLayout&                                    pipelineLayout, 
								       Device&                                            device):device(device)
{

}

mini::RayTracingPipeline::RayTracingPipeline(Device& device, VkRayTracingPipelineCreateInfoKHR& createInfo):device(device)
{
	vkCreateRayTracingPipelinesKHR(device.getHandle(),{},{},1,&createInfo,nullptr,&handle);
}

RayTracingPipeline::~RayTracingPipeline()
{
	if(handle!=VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device.getHandle(), handle, nullptr);
	}
}