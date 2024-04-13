#include "makeCubeMapPipeline.h"

mini::MakeCubeMapPipeline::MakeCubeMapPipeline(Device& device, ResourceManager& resourceManager, PushConstantRaster& pcRaster)
	: device(device), resourceManager(resourceManager), pcRaster(pcRaster)
{
	createHDRToCubeMapUniforms();

	std::vector<SubpassInfo> subpassInfos;


	// hdrToCubemap
	{
		SubpassInfo subpassInfo = {};
		subpassInfo.output.push_back(0);

		VkSubpassDependency dependency = {};

		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


		subpassInfo.dependencies.push_back(dependency);

		subpassInfos.push_back(subpassInfo);
	}

	createDescriptorSetLayout();

	std::vector<Attachment> attachments;
	{


		Attachment attachment{ CUBE_MAP_FORMAT,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(attachment);

	}

	std::vector<LoadStoreInfo> loadStoreInfos;
	{

		LoadStoreInfo depthLoadStoreInfo{};
		depthLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;


		// for shadow map
		loadStoreInfos.push_back(depthLoadStoreInfo);
	}


	VkExtent2D surfaceExtent = { SHADOW_WIDTH,SHADOW_HEIGHT };

	renderPass = std::make_unique<RenderPass>(device, attachments, loadStoreInfos, subpassInfos);



	hdrToCubeMapRenderPass = std::make_unique<HDRToCubeMapRenderPass>(device, resourceManager, *renderPass, surfaceExtent, descSetLayout, pcRaster, 0);

	createRenderTarget();

}

mini::MakeCubeMapPipeline::~MakeCubeMapPipeline()
{

}

void mini::MakeCubeMapPipeline::createHDRToCubeMapUniforms()
{
	hdrToCubeMapUniformsData.proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	hdrToCubeMapUniformsData.view[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	hdrToCubeMapUniformsData.view[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	hdrToCubeMapUniformsData.view[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	hdrToCubeMapUniformsData.view[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	hdrToCubeMapUniformsData.view[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	hdrToCubeMapUniformsData.view[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));


	//create buffer
	hdrToCubeMapUniforms = std::make_unique<Buffer>(device, sizeof(hdrToCubeMapUniformsData), reinterpret_cast<void*>(&hdrToCubeMapUniformsData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void mini::MakeCubeMapPipeline::createDescriptorSetLayout()
{
	descSetBindings.addBinding(HDRToCubeMapBindings::eHDRInput, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);


	descSetBindings.addBinding(HDRToCubeMapBindings::eHDRToCubeMapUniforms, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);


	descSetLayout = descSetBindings.createLayout(device);
	descPool = descSetBindings.createPool(device, 1);
	descSet = descPool->allocateDescriptorSet(*descSetLayout);

}

void mini::MakeCubeMapPipeline::createRenderTarget()
{




	VkExtent2D cubeMapExtent = { CUBE_MAP_SIZE,CUBE_MAP_SIZE };


	std::vector<Image> images;

	std::unique_ptr<Image> image = std::make_unique<Image>(device,
		cubeMapExtent, CUBE_MAP_FORMAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 6);


	images.push_back(std::move(*image));

	renderTarget = std::make_unique<RenderTarget>(std::move(images), VK_IMAGE_VIEW_TYPE_CUBE);

	framebuffer = std::make_unique<FrameBuffer>(device, *renderTarget, *renderPass);




}

void mini::MakeCubeMapPipeline::updateDescriptorSet(ImageView& hdrImageView)
{
	std::vector<VkWriteDescriptorSet> descriptorWrites;



	VkDescriptorImageInfo hdrInputImageInfo;
	hdrInputImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	hdrInputImageInfo.imageView = hdrImageView.getHandle();
	hdrInputImageInfo.sampler = resourceManager.getDefaultSampler().getHandle();

	descriptorWrites.emplace_back(descSetBindings.makeWrite(descSet, HDRToCubeMapBindings::eHDRInput, &hdrInputImageInfo));

	VkDescriptorBufferInfo hdrToCubeMapUniformsInfo = hdrToCubeMapUniforms->getDescriptorBufferInfo();
	descriptorWrites.emplace_back(descSetBindings.makeWrite(descSet, HDRToCubeMapBindings::eHDRToCubeMapUniforms, &hdrToCubeMapUniformsInfo));

	vkUpdateDescriptorSets(device.getHandle(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void mini::MakeCubeMapPipeline::draw(CommandBuffer& cmd)
{
	VkClearValue clearValue;
	clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };


	cmd.beginRenderPass(*renderPass, *framebuffer, { clearValue });
	hdrToCubeMapRenderPass->draw(cmd, { descSet });
	cmd.endRenderPass();
}
