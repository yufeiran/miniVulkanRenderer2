#include "shadowPipelineBuilder.h"

using namespace mini;

ShadowPipelineBuilder::ShadowPipelineBuilder(Device& device, ResourceManager& resourceManager,PushConstantRaster& pcRaster)
	:device(device),resourceManager(resourceManager),pcRaster(pcRaster)
{
	
	std::vector<SubpassInfo> subpassInfos;


	// shadow map pass
	{
		SubpassInfo subpassInfo = {};
		subpassInfo.output.push_back(-1);
		subpassInfo.output.push_back(0);

		VkSubpassDependency shadowDependency = {};

		shadowDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		shadowDependency.dstSubpass = 0;
		shadowDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		shadowDependency.srcAccessMask = 0;
		shadowDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		shadowDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		subpassInfo.dependencies.push_back(shadowDependency);

		subpassInfos.push_back(subpassInfo);
	}
	 
	createDescriptorSetLayout();

	std::vector<Attachment> attachments;
	{

		Attachment depthAttachment{device.getPhysicalDevice().findDepthFormat(),VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT};
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout   = VK_IMAGE_LAYOUT_GENERAL;


		// for shadow map
		attachments.push_back(depthAttachment);
	}

	std::vector<LoadStoreInfo> loadStoreInfos;
	{

		LoadStoreInfo depthLoadStoreInfo{};
		depthLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	

		// for shadow map
		loadStoreInfos.push_back(depthLoadStoreInfo);
	}


	VkExtent2D surfaceExtent = {SHADOW_WIDTH,SHADOW_HEIGHT};

	renderPass     = std::make_unique<RenderPass>(device,attachments,loadStoreInfos,subpassInfos);


	shadowMapRenderPass  = std::make_unique<ShadowMapRenderPass>(device,resourceManager,*renderPass,surfaceExtent,descSetLayout,pcRaster,0);

	createRenderTarget();

}

ShadowPipelineBuilder::~ShadowPipelineBuilder()
{

}

void ShadowPipelineBuilder::createDescriptorSetLayout()
{

	descSetLayout = descSetBindings.createLayout(device);
	descPool      = descSetBindings.createPool(device,1);
	descSet       = descPool->allocateDescriptorSet(*descSetLayout);
}

void ShadowPipelineBuilder::createRenderTarget()
{
	
	auto depthFormat = device.getPhysicalDevice().findDepthFormat();



	VkExtent2D shadowExtent = {SHADOW_WIDTH,SHADOW_HEIGHT};

	std::vector<Image> images;

	std::unique_ptr<Image> shadowDepthImage = std::make_unique<Image>(device,
		 shadowExtent, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT);


	images.push_back(std::move(*shadowDepthImage));


	
	renderTarget = std::make_unique<RenderTarget>(std::move(images));

	framebuffer = std::make_unique<FrameBuffer>(device,*renderTarget,*renderPass);


}

void ShadowPipelineBuilder::draw(CommandBuffer& cmd)
{
	std::vector<VkClearValue> clearValues(1);

	clearValues[0].depthStencil = { 1.0f,0 };

	cmd.beginRenderPass(*renderPass, *framebuffer,clearValues);
	shadowMapRenderPass->draw(cmd,descSet);

	cmd.endRenderPass();


}
