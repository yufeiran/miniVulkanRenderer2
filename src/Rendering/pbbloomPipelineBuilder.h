#pragma once 

#include "Common/common.h"
#include "Common/light.h"
#include "Vulkan/shaderModule.h"
#include "Vulkan/descriptorSetBindings.h"
#include "Vulkan/descriptorSetLayout.h"
#include "Vulkan/pipelineLayout.h"
#include "Vulkan/renderPass.h"
#include "Vulkan/graphicsPipeline.h"
#include "Rendering/renderContext.h"
#include "Rendering/graphicsRenderPass.h"
#include "Vulkan/buffer.h"

namespace mini
{

class Device;
class ResourceManager;
class DescriptorSetBindings;
class DescriptorSetLayout;
class DescriptorPool;



class pbbloomPipelineBuilder
{
public:
	pbbloomPipelineBuilder(Device& device, ResourceManager& resourceManager,
		VkExtent2D extent,
		std::shared_ptr<DescriptorSetLayout> descSetLayout,
		RenderTarget                          &gBufferRenderTarget,
		VkFormat offscreenColorFormat,
		PushConstantRaster& pcRaster,
		int N);
	~pbbloomPipelineBuilder();

	void createDescriptorSetLayout();
	

	void updateDescriptorSet();


	void draw(CommandBuffer& cmd,const VkDescriptorSet& descSet);

	void createRenderTarget();

private:
	VkExtent2D							  extent;
	int                                   N;

	VkFormat                              offscreenColorFormat;


	Device&                               device;		
	ResourceManager&                      resourceManager;
	DescriptorSetBindings                 descSetBindings;
	std::shared_ptr<DescriptorSetLayout>  descSetLayout;
	std::unique_ptr<DescriptorPool>       descPool;
	VkDescriptorSet                       descSet;


	std::unique_ptr<PipelineLayout>       pipelineLayout;
	std::unique_ptr<RenderPass>           renderPass;

	std::unique_ptr<SSAORenderPass>       pbbRenderPass;


	PushConstantRaster&                   pcRaster;

	std::unique_ptr<RenderTarget>         renderTarget;
	std::unique_ptr<FrameBuffer>          framebuffer;



	DescriptorSetBindings                 pbbDescSetBindings;
	std::shared_ptr<DescriptorSetLayout>  pbbDescSetLayout;
	std::unique_ptr<DescriptorPool>       pbbDescPool;
	VkDescriptorSet                       pbbDescSet;



	RenderTarget                          &gBufferRenderTarget;

};

}