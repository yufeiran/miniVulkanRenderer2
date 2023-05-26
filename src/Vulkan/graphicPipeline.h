#pragma once

#include"vk_common.h"
namespace mini
{
class Device;
class ShaderModule;

class GraphicPipeline
{
public:
	GraphicPipeline(std::vector<std::unique_ptr<ShaderModule>>& shaderModules,Device& device, VkExtent2D extent,
		VkFormat swapChainImageFormat);
	~GraphicPipeline();
	void createRenderPass();
private:
	Device& device;
	std::vector<std::unique_ptr<ShaderModule>>& shaderModules;
	VkPipeline handle{ VK_NULL_HANDLE };
	VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
	VkRenderPass renderPass{ VK_NULL_HANDLE };
	VkFormat swapChainImageFormat;
};
}