#include <random>
#include "SSRPipelineBuilder.h"

using namespace mini;



mini::SSRPipelineBuilder::SSRPipelineBuilder(Device& device, ResourceManager& resourceManager,VkExtent2D extent,
	std::shared_ptr<DescriptorSetLayout> descSetLayout,
	RenderTarget                          &gBufferRenderTarget,
	VkFormat offscreenColorFormat,
	PushConstantRaster& pcRaster)
	: device(device),
	resourceManager(resourceManager),
	pcRaster(pcRaster),
	gBufferRenderTarget(&gBufferRenderTarget)
{
	std::vector<SubpassInfo> subpassInfos;

	this->offscreenColorFormat = offscreenColorFormat;

	this->extent = extent;
	// SSR pass
	{
		SubpassInfo subpassInfo = {};

		subpassInfo.output.push_back(0);
		subpassInfo.output.push_back(-1);

		VkSubpassDependency shadowDependency = {};

		shadowDependency.srcSubpass = 0;
		shadowDependency.dstSubpass = 1;
		shadowDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		shadowDependency.srcAccessMask = 0;
		shadowDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		shadowDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		subpassInfo.dependencies.push_back(shadowDependency);

		subpassInfos.push_back(subpassInfo);
	}

	{

	
		SubpassInfo subpassInfo = {};


		subpassInfo.output.push_back(1);
		subpassInfo.output.push_back(-1);

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

	std::vector<Attachment> attachments;
	{

		Attachment ssaoAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		ssaoAttachment.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
		ssaoAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(ssaoAttachment);

		Attachment ssaoBlurAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		ssaoBlurAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ssaoBlurAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(ssaoBlurAttachment);

	}

	std::vector<LoadStoreInfo> loadStoreInfos;
	{

		LoadStoreInfo ssaoLoadStoreInfo{};

		ssaoLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		ssaoLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(ssaoLoadStoreInfo);

		LoadStoreInfo ssaoBlurLoadStoreInfo{};

		ssaoBlurLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		ssaoBlurLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(ssaoBlurLoadStoreInfo);

	}




	renderPass  = std::make_unique<RenderPass>(device,attachments,loadStoreInfos,subpassInfos);
		 

	createRenderTarget();
	createDescriptorSetLayout();

	ssrRenderPass = std::make_unique<SSRRenderPass>(device,resourceManager,*renderPass,extent,descSetLayout,ssrDescSetLayout, pcRaster,0);
	ssrBlurRenderPass = std::make_unique<SSRBlurRenderPass>(device,resourceManager,*renderPass,extent,ssaoBlurDescSetLayout, pcRaster,1);



	createSSAOBuffers();

	updateDescriptorSet();


	
}

mini::SSRPipelineBuilder::~SSRPipelineBuilder()
{
}


void SSRPipelineBuilder::createDescriptorSetLayout()
{
	ssrDescSetBindings.clear();
	ssaoBlurDescSetBindings.clear();


	// SSR
	ssrDescSetBindings.addBinding(SSRBindings::eSSRColor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	ssrDescSetBindings.addBinding(SSRBindings::eSSRDepth, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	ssrDescSetBindings.addBinding(SSRBindings::eSSRNormal, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	ssrDescSetBindings.addBinding(SSRBindings::eSSRPosition, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	ssrDescSetBindings.addBinding(SSRBindings::eSSRMetalRough,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

	ssrDescSetLayout = ssrDescSetBindings.createLayout(device);
	ssrDescPool = ssrDescSetBindings.createPool(device, 1);
	ssrDescSet = ssrDescPool->allocateDescriptorSet(*ssrDescSetLayout);

	// SSR Blur
	ssaoBlurDescSetBindings.addBinding(SSAOBlurBindings::eSSAOInput, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	ssaoBlurDescSetLayout = ssaoBlurDescSetBindings.createLayout(device);
	ssaoBlurDescPool = ssaoBlurDescSetBindings.createPool(device, 1);
	ssaoBlurDescSet = ssaoBlurDescPool->allocateDescriptorSet(*ssaoBlurDescSetLayout);
}

void SSRPipelineBuilder::updateDescriptorSet()
{
		// SSR
	VkDescriptorBufferInfo ssaoSampleBufferInfo{ ssaoSamplesBuffer->getHandle(), 0, VK_WHOLE_SIZE };
	std::vector<VkWriteDescriptorSet> ssrWrites;


	VkDescriptorImageInfo gBufferColorInfo;
	gBufferColorInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	gBufferColorInfo.imageView = gBufferRenderTarget->getImageViewByIndex(0).getHandle();
	gBufferColorInfo.sampler = resourceManager.getRepeatSampler().getHandle();

	VkDescriptorImageInfo gBufferNormalInfo;
	gBufferNormalInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	gBufferNormalInfo.imageView = gBufferRenderTarget->getImageViewByIndex(10).getHandle();
	gBufferNormalInfo.sampler = resourceManager.getRepeatSampler().getHandle();

	VkDescriptorImageInfo gBufferDpethInfo;
	gBufferDpethInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	gBufferDpethInfo.imageView = gBufferRenderTarget->getImageViewByIndex(1).getHandle();
	gBufferDpethInfo.sampler = resourceManager.getRepeatSampler().getHandle();

	VkDescriptorImageInfo gBufferPosInfo;
	gBufferPosInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	gBufferPosInfo.imageView = gBufferRenderTarget->getImageViewByIndex(9).getHandle();
	gBufferPosInfo.sampler = resourceManager.getRepeatSampler().getHandle();

	VkDescriptorImageInfo gBufferMetalRoughInfo;
	gBufferMetalRoughInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	gBufferMetalRoughInfo.imageView = gBufferRenderTarget->getImageViewByIndex(5).getHandle();
	gBufferMetalRoughInfo.sampler = resourceManager.getRepeatSampler().getHandle();


	ssrWrites.emplace_back(ssrDescSetBindings.makeWrite(ssrDescSet, SSRBindings::eSSRColor, &gBufferColorInfo));
	ssrWrites.emplace_back(ssrDescSetBindings.makeWrite(ssrDescSet, SSRBindings::eSSRDepth, &gBufferDpethInfo));
	ssrWrites.emplace_back(ssrDescSetBindings.makeWrite(ssrDescSet, SSRBindings::eSSRNormal, &gBufferNormalInfo));
	ssrWrites.emplace_back(ssrDescSetBindings.makeWrite(ssrDescSet, SSRBindings::eSSRPosition, &gBufferPosInfo));
	ssrWrites.emplace_back(ssrDescSetBindings.makeWrite(ssrDescSet, SSRBindings::eSSRMetalRough, &gBufferMetalRoughInfo));



	vkUpdateDescriptorSets(device.getHandle(), static_cast<uint32_t>(ssrWrites.size()), ssrWrites.data(), 0, nullptr);

	// SSAO Blur
	std::vector<VkWriteDescriptorSet> ssaoBlurWrites;
	
	VkDescriptorImageInfo ssaoInputInfo;
	ssaoInputInfo.imageLayout =   VK_IMAGE_LAYOUT_GENERAL;
	ssaoInputInfo.imageView = ssrRenderTarget->getImageViewByIndex(0).getHandle();
	ssaoInputInfo.sampler = resourceManager.getDefaultSampler().getHandle();
	ssaoBlurWrites.emplace_back(ssaoBlurDescSetBindings.makeWrite(ssaoBlurDescSet, SSAOBlurBindings::eSSAOInput, &ssaoInputInfo));

	vkUpdateDescriptorSets(device.getHandle(), static_cast<uint32_t>(ssaoBlurWrites.size()), ssaoBlurWrites.data(), 0, nullptr);


}



void SSRPipelineBuilder::draw(CommandBuffer& cmd,const VkDescriptorSet& descSet)
{
	std::vector<VkClearValue> clearValues(2);

	

	clearValues[0].color = { 0,0,0,1 };
	clearValues[1].color = { 0,0,0,1 };
	cmd.beginRenderPass(*renderPass,*ssrFramebuffer,clearValues);
	ssrRenderPass->draw(cmd,{descSet,ssrDescSet});

	cmd.nextSubpass();

	ssrBlurRenderPass->draw(cmd,{ssaoBlurDescSet});

	cmd.endRenderPass();


}

void SSRPipelineBuilder::createSSAOBuffers()
{
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	// Sample kernel

	std::vector<glm::vec3> ssaoKernel;
	for(unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(
					randomFloats(generator) * 2.0 - 1.0,
					randomFloats(generator) * 2.0 - 1.0,
					randomFloats(generator)
				);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	ssaoSamplesBuffer = std::make_unique<Buffer>(device, ssaoKernel, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT|VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

	std::vector<glm::vec3> ssaoNoise;
	for(unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}

	VkExtent2D extent = { 4, 4 };
	ssaoNoiseImage = std::make_unique<Image>(device,extent,ssaoNoise.size() * sizeof(glm::vec3),ssaoNoise.data(),VK_FORMAT_R8G8B8A8_UNORM);
	ssaoNoiseImageView = std::make_unique<ImageView>( *ssaoNoiseImage, VK_FORMAT_R8G8B8A8_UNORM);



}

void mini::SSRPipelineBuilder::createRenderTarget()
{
	ssrRenderTarget.reset();
	ssrFramebuffer.reset();

	{
		std::vector<Image> images;

		std::unique_ptr<Image> ssaoImage = std::make_unique<Image>(device,
			 extent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);

		std::unique_ptr<Image> ssaoBlurImage = std::make_unique<Image>(device,
			extent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);


		images.push_back(std::move(*ssaoImage));
		images.push_back(std::move(*ssaoBlurImage));



	
		ssrRenderTarget = std::make_unique<RenderTarget>(std::move(images));

		ssrFramebuffer = std::make_unique<FrameBuffer>(device,*ssrRenderTarget,*renderPass);
	}



}

void mini::SSRPipelineBuilder::rebuild(VkExtent2D extent, RenderTarget& gBufferRenderTarget)
{
	this->extent = extent;
	this->gBufferRenderTarget = &gBufferRenderTarget;

	createRenderTarget();
	createDescriptorSetLayout();

	
	ssrBlurRenderPass->rebuild(extent, 1);
	ssrRenderPass->rebuild(extent, 0);

	updateDescriptorSet();
}
