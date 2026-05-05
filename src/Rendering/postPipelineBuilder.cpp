#include"postPipelineBuilder.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

namespace mini
{
	void PostPipelineBuilder::initPostRender(Device& device, RenderTarget& offscreenRenderTarget, VkFormat postSurfaceColorFormat,
		VkExtent2D surfaceExtent, VkFormat swapChainFormat)
	{
		// create post pipeline!
		ShaderInfo postShaderInfo;
		postShaderInfo.bindingInfoMap[0][1] = BindingInfo{ TEXTURE_BINDING_TYPE,DIFFUSE };


		std::string vertPath = getSPVPath("post.vert.spv");
		std::string fragPath = getSPVPath("post.frag.spv");
		std::unique_ptr<ShaderModule> vertShaderModule = std::make_unique<ShaderModule>(vertPath, device, VK_SHADER_STAGE_VERTEX_BIT);
		std::unique_ptr<ShaderModule> fragShaderModule = std::make_unique<ShaderModule>(fragPath, device, VK_SHADER_STAGE_FRAGMENT_BIT);

		postShaderModules.emplace_back(std::move(vertShaderModule));
		postShaderModules.emplace_back(std::move(fragShaderModule));

		for (auto& s : postShaderModules)
		{
			s->setShaderInfo(postShaderInfo);
		}

		std::vector<VkPushConstantRange> pushConstants;

		VkPushConstantRange pushConstant = {};
		pushConstant.offset = 0;
		pushConstant.size = sizeof(PushConstantPost);
		pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstants.push_back(pushConstant);

		postDescSetBind.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
		postDescSetBind.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
		postDescSetBind.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
		postDescSetBind.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);  // debug SSR


		postDescriptorSetLayouts.push_back(std::move(postDescSetBind.createLayout(device)));

		postPipelineLayout = std::make_unique<PipelineLayout>(device, postDescriptorSetLayouts, pushConstants);

		std::vector<Attachment> attachments;
		{
			Attachment colorAttachment{ postSurfaceColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			attachments.push_back(colorAttachment);

			Attachment depthAttachment{ device.getPhysicalDevice().findDepthFormat(),VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT };
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			attachments.push_back(depthAttachment);


		}

		std::vector<LoadStoreInfo> loadStoreInfos;
		{
			LoadStoreInfo colorLoadStoreInfo{};
			colorLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			loadStoreInfos.push_back(colorLoadStoreInfo);

			LoadStoreInfo depthLoadStoreInfo{};
			depthLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			loadStoreInfos.push_back(depthLoadStoreInfo);


		}

		postRenderPass = std::make_unique<RenderPass>(device, attachments, loadStoreInfos);
		postPipeline = std::make_unique<GraphicsPipeline>(postShaderModules, *postPipelineLayout, device, surfaceExtent);
		postPipeline->rasterizer.cullMode = VK_CULL_MODE_NONE;
		postPipeline->build(*postRenderPass);

		// create quad
		postQuad = std::make_unique<PostQuad>(device);

		// create descriptor set
		postDescriptorPool = std::make_unique<DescriptorPool>(device);
		BindingMap<VkDescriptorImageInfo> imageInfos;
		BindingMap<VkDescriptorBufferInfo> bufferInfos;

		const auto& offscreenColorImageView = offscreenRenderTarget.getImageViewByIndex(0);
		const auto& offscreenColorImage = offscreenRenderTarget.getImageByIndex(0);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		postRenderImageSampler = std::make_unique<Sampler>(device);
		imageInfo.imageView = offscreenColorImageView.getHandle();


		imageInfo.sampler = postRenderImageSampler->getHandle();
		imageInfos[0][1] = imageInfo;

		postDescriptorSet = postDescriptorPool->allocate(*postDescriptorSetLayouts[0], bufferInfos, imageInfos).getHandle();

		VkDescriptorSetLayoutBinding postSamplerLayoutBinding{};
		postSamplerLayoutBinding.binding = 1;
		postSamplerLayoutBinding.descriptorCount = 1;
		postSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		postSamplerLayoutBinding.pImmutableSamplers = nullptr;
		postSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding postSamplerShadowMapLayoutBinding{};
		postSamplerLayoutBinding.binding = 2;
		postSamplerLayoutBinding.descriptorCount = 1;
		postSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		postSamplerLayoutBinding.pImmutableSamplers = nullptr;
		postSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding postSamplerBloomLayoutBinding{};
		postSamplerBloomLayoutBinding.binding = 3;
		postSamplerBloomLayoutBinding.descriptorCount = 1;
		postSamplerBloomLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		postSamplerBloomLayoutBinding.pImmutableSamplers = nullptr;
		postSamplerBloomLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding postSamplerSSRLayoutBinding{};
		postSamplerSSRLayoutBinding.binding = 4;
		postSamplerSSRLayoutBinding.descriptorCount = 1;
		postSamplerSSRLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		postSamplerSSRLayoutBinding.pImmutableSamplers = nullptr;
		postSamplerSSRLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


		std::vector<VkDescriptorSetLayoutBinding>postLayoutBindings{ postSamplerLayoutBinding,postSamplerShadowMapLayoutBinding,postSamplerBloomLayoutBinding,postSamplerSSRLayoutBinding };

		postDescriptorSetLayouts.push_back(std::make_unique<DescriptorSetLayout>(device, postLayoutBindings));

	}


	void PostPipelineBuilder::updatePostDescriptorSet(Device& device, const PostInputTextures& postInputTextures)
	{
		std::vector<VkWriteDescriptorSet> writes;

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo.imageView = postInputTextures.offscreenColor.getHandle();
		imageInfo.sampler = postRenderImageSampler->getHandle();
		VkWriteDescriptorSet writeDescriptorSets = postDescSetBind.makeWrite(postDescriptorSet, 1, &imageInfo);

		writes.push_back(writeDescriptorSets);

		VkDescriptorImageInfo shadowMapInfo;
		shadowMapInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		shadowMapInfo.imageView = postInputTextures.shadowMap.getHandle();
		shadowMapInfo.sampler = postInputTextures.defaultSampler.getHandle();

		writes.push_back(postDescSetBind.makeWrite(postDescriptorSet, 2, &shadowMapInfo));

		VkDescriptorImageInfo imageBloomInfo{};
		imageBloomInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageBloomInfo.imageView = postInputTextures.bloomTexture.getHandle();
		imageBloomInfo.sampler = postRenderImageSampler->getHandle();

		writes.push_back(postDescSetBind.makeWrite(postDescriptorSet, 3, &imageBloomInfo));

		VkDescriptorImageInfo imageSSRInfo{};
		imageSSRInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageSSRInfo.imageView = postInputTextures.ssrTexture.getHandle();
		imageSSRInfo.sampler = postRenderImageSampler->getHandle();

		writes.push_back(postDescSetBind.makeWrite(postDescriptorSet, 4, &imageSSRInfo));

		vkUpdateDescriptorSets(device.getHandle(), writes.size(), writes.data(), 0, nullptr);
	}

	void PostPipelineBuilder::drawPostBegin(CommandBuffer& cmd, const std::vector<VkClearValue>& clearValues,
		FrameBuffer& frameBuffer, PushConstantPost& pcPost)
	{
		cmd.beginRenderPass(*postRenderPass, frameBuffer, clearValues);
		cmd.bindPipeline(*postPipeline);

		cmd.bindDescriptorSet({ postDescriptorSet });
		//cmd.pushConstant(pcPost,static_cast<VkShaderStageFlagBits>( ));

		//void CommandBuffer::pushConstant(PushConstantPost& pushConstant,VkShaderStageFlagBits stage)

		vkCmdPushConstants(cmd.getHandle(), postPipelineLayout->getHandle(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantPost), &pcPost);
		cmd.bindVertexBuffer(postQuad->getVertexBuffer());

		cmd.setViewPortAndScissor(frameBuffer.getExtent());

		cmd.draw(3, 1, 0, 0);
		cmd.draw(3, 1, 1, 0);

		// ImGui 
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();



	}

	void PostPipelineBuilder::drawPostEnd(CommandBuffer& cmd, const std::vector<VkClearValue>& clearValues,
		FrameBuffer& frameBuffer, PushConstantPost& pcPost)
	{

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd.getHandle());

		cmd.endRenderPass();
	}

}