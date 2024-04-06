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



class SSRPipelineBuilder
{
public:
	SSRPipelineBuilder(Device& device, ResourceManager& resourceManager,
		VkExtent2D extent,
		std::shared_ptr<DescriptorSetLayout> descSetLayout,
		RenderTarget                          &gBufferRenderTarget,
		VkFormat offscreenColorFormat,
		PushConstantRaster& pcRaster);
	~SSRPipelineBuilder();

	void createDescriptorSetLayout();
	

	void updateDescriptorSet();


	void draw(CommandBuffer& cmd,const VkDescriptorSet& descSet);

	void createSSAOBuffers();

	void createRenderTarget();

	void rebuild(VkExtent2D extent, RenderTarget& gBufferRenderTarget);

	RenderTarget& getRenderTarget() { return *ssrRenderTarget; }

private:
	VkExtent2D							  extent;

	VkFormat offscreenColorFormat;


	Device&                               device;		
	ResourceManager&                      resourceManager;
	DescriptorSetBindings                 descSetBindings;
	std::shared_ptr<DescriptorSetLayout>  descSetLayout;
	std::unique_ptr<DescriptorPool>       descPool;
	VkDescriptorSet                       descSet;


	std::unique_ptr<PipelineLayout>       pipelineLayout;
	std::unique_ptr<RenderPass>           renderPass;
	std::unique_ptr<RenderPass>		      blurRenderPass;

	std::unique_ptr<SSRRenderPass>        ssrRenderPass;
	std::unique_ptr<SSRBlurRenderPass>    ssrBlurRenderPass;

	PushConstantRaster&                   pcRaster;

	std::unique_ptr<RenderTarget>         ssrRenderTarget;
	std::unique_ptr<FrameBuffer>          ssrFramebuffer;



	DescriptorSetBindings                 ssrDescSetBindings;
	std::shared_ptr<DescriptorSetLayout>  ssrDescSetLayout;
	std::unique_ptr<DescriptorPool>       ssrDescPool;
	VkDescriptorSet                       ssrDescSet;

	DescriptorSetBindings                 ssaoBlurDescSetBindings;
	std::shared_ptr<DescriptorSetLayout>  ssaoBlurDescSetLayout;
	std::unique_ptr<DescriptorPool>       ssaoBlurDescPool;
	VkDescriptorSet                       ssaoBlurDescSet;

	std::unique_ptr<Buffer>               ssaoSamplesBuffer;
	std::unique_ptr<Image> 			      ssaoNoiseImage;
	std::unique_ptr<ImageView>            ssaoNoiseImageView;


	RenderTarget                          *gBufferRenderTarget;

};

}