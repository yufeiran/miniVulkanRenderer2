#pragma once

#include"Common/common.h"
#include"Vulkan/pipelineLayout.h"
namespace mini
{
class Device;
class ShaderModule;
class RenderPass;
class Buffer;
class DescriptorSetLayout;
class PipelineLayout;


class GraphicsPipeline
{
public:
	GraphicsPipeline(std::vector<std::unique_ptr<ShaderModule>>& shaderModules, 
		PipelineLayout &pipelineLayout, 
		Device& device, VkExtent2D extent);
	~GraphicsPipeline();

	void build(const RenderPass& renderPass);

	void setSubpassIndex(int index){subpassIndex = index;}


	VkPipelineLayout getPipelineLayout() const;

	VkPipeline getHandle() const;


	std::vector<std::unique_ptr<ShaderModule>>& getShaderModules()const;



	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	VkVertexInputBindingDescription bindingDescription;


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
	PipelineLayout &pipelineLayout;
	

private:
	Device& device;
	std::vector<std::unique_ptr<ShaderModule>>& shaderModules;
	VkPipeline handle{ VK_NULL_HANDLE };
	
	std::unique_ptr<Buffer> vertexBuffer;

	VkFormat swapChainImageFormat;

	std::vector<std::unique_ptr<Buffer>>uniformBuffers;
	std::vector<void*>uniformBuffersMapped;

	int subpassIndex = 0;

public:
	std::vector<VkDynamicState> dynamicStates; 
};
}