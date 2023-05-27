#pragma once

#include"vk_common.h"
namespace mini
{
class Device;
class ShaderModule;
class RenderPass;

class GraphicPipeline
{
public:
	GraphicPipeline(std::vector<std::unique_ptr<ShaderModule>>& shaderModules,Device& device, VkExtent2D extent,
		VkFormat swapChainImageFormat);
	~GraphicPipeline();

	RenderPass& getRenderPass() const;
private:
	Device& device;
	std::vector<std::unique_ptr<ShaderModule>>& shaderModules;
	std::unique_ptr<RenderPass> renderPass;
	VkPipeline handle{ VK_NULL_HANDLE };
	VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };

	VkFormat swapChainImageFormat;
};
}