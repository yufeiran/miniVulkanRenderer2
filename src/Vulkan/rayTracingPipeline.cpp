#include "Vulkan/rayTracingPipeline.h"

using namespace mini;

RayTracingPipeline::RayTracingPipeline(std::vector<VkRayTracingShaderGroupCreateInfoKHR>& shaderGroup,
								       PipelineLayout&                                    pipelineLayout, 
								       Device&                                            device):device(device)
{

}

RayTracingPipeline::~RayTracingPipeline()
{
	if(handle!=VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device.getHandle(), handle, nullptr);
	}
}