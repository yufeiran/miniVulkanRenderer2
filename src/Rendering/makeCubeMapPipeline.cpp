#include "makeCubeMapPipeline.h"

mini::MakeCubeMapPipeline::MakeCubeMapPipeline(Device& device, ResourceManager& resourceManager, PushConstantRaster& pcRaster)
	: device(device), resourceManager(resourceManager), pcRaster(pcRaster)
{




	createHDRToCubeMapUniforms();

	createDescriptorSetLayout();




	// hdrToCubemap
	{
		std::vector<SubpassInfo> subpassInfos;
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



		std::vector<Attachment> attachments;
		{

			// cubemap
			Attachment attachment{ CUBE_MAP_FORMAT,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
			attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

			attachments.push_back(attachment);



		}

		std::vector<LoadStoreInfo> loadStoreInfos;
		{

			LoadStoreInfo cubemapLoadStoreInfo{};
			cubemapLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			cubemapLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			// for cubemap
			loadStoreInfos.push_back(cubemapLoadStoreInfo);


		}





		hdrToCubeMapRenderPass = std::make_unique<RenderPass>(device, attachments, loadStoreInfos, subpassInfos);



		hdrToCubeMapPass = std::make_unique<HDRToCubeMapRenderPass>(device, resourceManager, *hdrToCubeMapRenderPass, cubeMapExtent, hdrToCubeMapDescSetLayout, pcRaster, 0);



	}


	// diffuse irradiance
	{
		std::vector<SubpassInfo> subpassInfos;
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



		std::vector<Attachment> attachments;
		{



			// irradiance map
			Attachment irradianceAttachment{ CUBE_MAP_FORMAT,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
			irradianceAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			irradianceAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

			attachments.push_back(irradianceAttachment);

		}

		std::vector<LoadStoreInfo> loadStoreInfos;
		{


			LoadStoreInfo irradianceCubemapLoadStoreInfo{};
			irradianceCubemapLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			irradianceCubemapLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			loadStoreInfos.push_back(irradianceCubemapLoadStoreInfo);

		}





		diffuseIrradianceRenderPass = std::make_unique<RenderPass>(device, attachments, loadStoreInfos, subpassInfos);


		diffuseIrradiancePass = std::make_unique<DiffuseIrradianceRenderPass>(device, resourceManager, *diffuseIrradianceRenderPass, diffuseIrradianceExtent, diffuseIrradianceDescSetLayout, pcRaster, 0);
	}





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

	// HDR to CubeMap
	{
		hdrToCubeMapDescSetBindings.addBinding(HDRToCubeMapBindings::eHDRInput, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);


		hdrToCubeMapDescSetBindings.addBinding(HDRToCubeMapBindings::eHDRToCubeMapUniforms, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);


		hdrToCubeMapDescSetLayout = hdrToCubeMapDescSetBindings.createLayout(device);
		hdrToCubeMapDescPool = hdrToCubeMapDescSetBindings.createPool(device, 1);
		hdrToCubeMapDescSet = hdrToCubeMapDescPool->allocateDescriptorSet(*hdrToCubeMapDescSetLayout);
	}

	// Diffuse Irradiance
	{
		diffuseIrradianceDescSetBindings.addBinding(HDRToCubeMapBindings::eHDRInput, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);


		diffuseIrradianceDescSetBindings.addBinding(HDRToCubeMapBindings::eHDRToCubeMapUniforms, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);


		diffuseIrradianceDescSetLayout = diffuseIrradianceDescSetBindings.createLayout(device);
		diffuseIrradianceDescPool = diffuseIrradianceDescSetBindings.createPool(device, 1);
		diffuseIrradianceDescSet = diffuseIrradianceDescPool->allocateDescriptorSet(*diffuseIrradianceDescSetLayout);
	}
}

void mini::MakeCubeMapPipeline::createRenderTarget()
{




	

	// HDR to CubeMap
	{


		std::vector<Image> images;

		std::unique_ptr<Image> image = std::make_unique<Image>(device,
			cubeMapExtent, CUBE_MAP_FORMAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 6);



		images.push_back(std::move(*image));


		hdrToCubeMapRenderTarget = std::make_unique<RenderTarget>(std::move(images), VK_IMAGE_VIEW_TYPE_CUBE);

		hdrToCubeMapFramebuffer = std::make_unique<FrameBuffer>(device, *hdrToCubeMapRenderTarget, *hdrToCubeMapRenderPass);
	}

	// Diffuse iradiance
	{


		std::vector<Image> images;


		std::unique_ptr<Image> irradianceMapimage = std::make_unique<Image>(device,
			diffuseIrradianceExtent, CUBE_MAP_FORMAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 6);

		images.push_back(std::move(*irradianceMapimage));

		diffuseIrradianceRenderTarget = std::make_unique<RenderTarget>(std::move(images), VK_IMAGE_VIEW_TYPE_CUBE);

		diffuseIrradianceFramebuffer = std::make_unique<FrameBuffer>(device, *diffuseIrradianceRenderTarget, *diffuseIrradianceRenderPass);

	}



}

void mini::MakeCubeMapPipeline::updateDescriptorSet(ImageView& hdrImageView)
{

	// HDR to cubemap
	{
		std::vector<VkWriteDescriptorSet> descriptorWrites;



		VkDescriptorImageInfo hdrInputImageInfo;
		hdrInputImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		hdrInputImageInfo.imageView = hdrImageView.getHandle();
		hdrInputImageInfo.sampler = resourceManager.getDefaultSampler().getHandle();

		descriptorWrites.emplace_back(hdrToCubeMapDescSetBindings.makeWrite(hdrToCubeMapDescSet, HDRToCubeMapBindings::eHDRInput, &hdrInputImageInfo));

		VkDescriptorBufferInfo hdrToCubeMapUniformsInfo = hdrToCubeMapUniforms->getDescriptorBufferInfo();
		descriptorWrites.emplace_back(hdrToCubeMapDescSetBindings.makeWrite(hdrToCubeMapDescSet, HDRToCubeMapBindings::eHDRToCubeMapUniforms, &hdrToCubeMapUniformsInfo));

		vkUpdateDescriptorSets(device.getHandle(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	// Diffuse Irradiance
	{
		std::vector<VkWriteDescriptorSet> descriptorWrites;

		VkDescriptorImageInfo hdrInputImageInfo;
		hdrInputImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		hdrInputImageInfo.imageView = hdrToCubeMapRenderTarget->getImageViewByIndex(0).getHandle();
		hdrInputImageInfo.sampler = resourceManager.getDefaultSampler().getHandle();

		descriptorWrites.emplace_back(diffuseIrradianceDescSetBindings.makeWrite(diffuseIrradianceDescSet, DiffuseIrradianceBindings::eDiffuseIrradianceInput, &hdrInputImageInfo));

		VkDescriptorBufferInfo hdrToCubeMapUniformsInfo = hdrToCubeMapUniforms->getDescriptorBufferInfo();
		descriptorWrites.emplace_back(diffuseIrradianceDescSetBindings.makeWrite(diffuseIrradianceDescSet, DiffuseIrradianceBindings::eDiffuseIrradianceUniforms, &hdrToCubeMapUniformsInfo));

		vkUpdateDescriptorSets(device.getHandle(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	}

}

void mini::MakeCubeMapPipeline::draw(CommandBuffer& cmd)
{
	std::vector<VkClearValue> clearValue{ 1 };
	clearValue[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };


	cmd.beginRenderPass(*hdrToCubeMapRenderPass, *hdrToCubeMapFramebuffer, clearValue);
	hdrToCubeMapPass->draw(cmd, { hdrToCubeMapDescSet });
	cmd.endRenderPass();

	cmd.beginRenderPass(*diffuseIrradianceRenderPass, *diffuseIrradianceFramebuffer, clearValue);
	diffuseIrradiancePass->draw(cmd, { diffuseIrradianceDescSet });
	cmd.endRenderPass();


}
