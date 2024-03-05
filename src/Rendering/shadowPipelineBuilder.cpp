#include "shadowPipelineBuilder.h"

using namespace mini;

ShadowPipelineBuilder::ShadowPipelineBuilder(Device& device, ResourceManager& resourceManager,PushConstantRaster& pcRaster,Buffer&  lightUniformsBuffer)
	:device(device),resourceManager(resourceManager),pcRaster(pcRaster),lightUniformsBuffer(lightUniformsBuffer)
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

	dirShadowRenderPass  = std::make_unique<RenderPass>(device,attachments,loadStoreInfos,subpassInfos);

	pointShadowRenderPass  = std::make_unique<RenderPass>(device,attachments,loadStoreInfos,subpassInfos);


	shadowMapRenderPass  = std::make_unique<DirShadowMapRenderPass>(device,resourceManager,*dirShadowRenderPass,surfaceExtent,descSetLayout,pcRaster,0);

	pointShadowMapRenderPass = std::make_unique<PointShadowMapRenderPass>(device,resourceManager,*pointShadowRenderPass,surfaceExtent,descSetLayout,pcRaster,0);

	createRenderTarget();

	updateDescriptorSet();

}

ShadowPipelineBuilder::~ShadowPipelineBuilder()
{

}

void ShadowPipelineBuilder::createDescriptorSetLayout()
{
	descSetBindings.addBinding(SceneBindings::eLight,  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
									VK_SHADER_STAGE_VERTEX_BIT |VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR |  VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
	

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


	
	dirShadowRenderTarget = std::make_unique<RenderTarget>(std::move(images));

	dirShadowFramebuffer = std::make_unique<FrameBuffer>(device,*dirShadowRenderTarget,*dirShadowRenderPass);


	std::vector<Image> pointShadowImages;

	std::unique_ptr<Image> pointShadowDepthImage = std::make_unique<Image>(device,
		 shadowExtent, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT,6);


	pointShadowImages.push_back(std::move(*pointShadowDepthImage));

	pointShadowRenderTarget = std::make_unique<RenderTarget>(std::move(pointShadowImages),VK_IMAGE_VIEW_TYPE_CUBE);

	pointShadowFramebuffer = std::make_unique<FrameBuffer>(device,*pointShadowRenderTarget,*pointShadowRenderPass);

	

}

void ShadowPipelineBuilder::updateDescriptorSet()
{

	std::vector<VkWriteDescriptorSet> writes;


	VkDescriptorBufferInfo lightUnif{lightUniformsBuffer.getHandle(), 0, VK_WHOLE_SIZE};
	writes.emplace_back(descSetBindings.makeWrite(descSet,SceneBindings::eLight,&lightUnif));


	vkUpdateDescriptorSets(device.getHandle(),static_cast<uint32_t>(writes.size()),writes.data(),0,nullptr);
}

void ShadowPipelineBuilder::draw(CommandBuffer& cmd)
{
	std::vector<VkClearValue> clearValues(1);

	

	clearValues[0].depthStencil = { 1.0f,0 };

	cmd.beginRenderPass(*dirShadowRenderPass, *dirShadowFramebuffer,clearValues);
	shadowMapRenderPass->draw(cmd,{descSet});

	cmd.endRenderPass();

	cmd.beginRenderPass(*pointShadowRenderPass, *pointShadowFramebuffer,clearValues);
	pointShadowMapRenderPass->draw(cmd,{descSet});

	cmd.endRenderPass();


}

