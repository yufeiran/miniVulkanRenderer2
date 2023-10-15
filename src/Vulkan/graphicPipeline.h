#pragma once

#include"Common/common.h"
namespace mini
{
class Device;
class ShaderModule;
class RenderPass;
class Buffer;
class DescriptorSetLayout;



class GraphicPipeline
{
public:
	GraphicPipeline(std::vector<std::unique_ptr<ShaderModule>>& shaderModules, std::vector<std::unique_ptr<DescriptorSetLayout>>&descriptorSetLayouts , Device& device, VkExtent2D extent,
		VkFormat swapChainImageFormat);
	~GraphicPipeline();

	void build(RenderPass& renderPass);


	VkPipelineLayout getPipelineLayout() const;

	VkPipeline getHandle() const;

	RenderPass& getRenderPass() const;

	std::vector<std::unique_ptr<ShaderModule>>& getShaderModules()const;


	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssembly;
	VkPipelineDynamicStateCreateInfo dynamicState;
	VkPipelineViewportStateCreateInfo viewportState;
	VkPipelineDepthStencilStateCreateInfo depthStencil;
	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineRasterizationStateCreateInfo rasterizer;
	VkPipelineMultisampleStateCreateInfo multisampling;
	VkPipelineColorBlendStateCreateInfo colorBlending;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };


private:
	Device& device;
	std::vector<std::unique_ptr<ShaderModule>>& shaderModules;
	std::vector<std::unique_ptr<DescriptorSetLayout>>& descriptorSetLayouts;
	std::unique_ptr<RenderPass> renderPass;
	VkPipeline handle{ VK_NULL_HANDLE };
	
	std::unique_ptr<Buffer> vertexBuffer;

	VkFormat swapChainImageFormat;

	std::vector<std::unique_ptr<Buffer>>uniformBuffers;
	std::vector<void*>uniformBuffersMapped;
};
}