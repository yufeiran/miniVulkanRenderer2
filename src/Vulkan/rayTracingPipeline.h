#pragma once
#include "Common/common.h"
#include "Vulkan/pipelineLayout.h"
#include "Vulkan/device.h"

namespace mini
{

class RayTracingPipeline
{
public:
	RayTracingPipeline(std::vector<VkRayTracingShaderGroupCreateInfoKHR>& shaderGroup,
		               PipelineLayout&                                    pipelineLayout, 
		               Device&                                            device);

	RayTracingPipeline(Device&                            device, 
					   VkRayTracingPipelineCreateInfoKHR& createInfo);

	~RayTracingPipeline();

	VkPipeline getHandle() { return handle;}
private:
	Device&    device;
	VkPipeline handle = VK_NULL_HANDLE;
};

}