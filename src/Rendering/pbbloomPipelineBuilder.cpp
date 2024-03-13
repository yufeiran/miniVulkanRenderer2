#include "pbbloomPipelineBuilder.h"

using namespace mini;

PBBloomPipelineBuilder::PBBloomPipelineBuilder(
	Device& device, 
	ResourceManager& resourceManager,
	VkExtent2D extent, 
	RenderTarget& gBufferRenderTarget, 
	VkFormat offscreenColorFormat, 
	PushConstantPost& pcPost,
	int N)
	: device(device),
	resourceManager(resourceManager),
	pcPost(pcPost),
	gBufferRenderTarget(&gBufferRenderTarget),N(N),offscreenColorFormat(offscreenColorFormat),extent(extent)
{


	rebuild(extent,gBufferRenderTarget,N);




	


	

	
}

PBBloomPipelineBuilder::~PBBloomPipelineBuilder()
{

}

void PBBloomPipelineBuilder::rebuild(VkExtent2D extent,RenderTarget& gBufferRenderTarget,  int N)
{
	this->extent = extent;
	this->gBufferRenderTarget = &gBufferRenderTarget;
	createDescriptorSetLayout();
	calculateBloomSizes();
	upSamplingRenderPasses.clear();
	downSamplingRenderPasses.clear();

	pbbDownSamplingRenderPasses.clear();
	pbbUpSamplingRenderPasses.clear();
	for(int i = 0; i < N; i++)
	{
		std::vector<SubpassInfo> subpassInfos;

			// downSamplePass
			{
				SubpassInfo subpassInfo = {};
				subpassInfo.output.push_back(-1);
				subpassInfo.output.push_back(0);

				VkSubpassDependency dependency = {};

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = 0;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				subpassInfo.dependencies.push_back(dependency);

				subpassInfos.push_back(subpassInfo);
			}
	 

			std::vector<Attachment> attachments;
			{

				Attachment attachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
				attachment.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
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


			auto downSamplingRenderPass  = std::make_unique<RenderPass>(device,attachments,loadStoreInfos,subpassInfos);
			downSamplingRenderPasses.push_back(std::move(downSamplingRenderPass));
			auto upSamplingRenderPass  = std::make_unique<RenderPass>(device,attachments,loadStoreInfos,subpassInfos);

			upSamplingRenderPasses.push_back(std::move(upSamplingRenderPass));

			VkExtent2D nowDownExtent = {bloomInputTextureSizes[i].x /2 , bloomInputTextureSizes[i].y /2 };

			auto pbbDownSamplingRenderPass = std::make_unique<PBBDownSamplingRenderPass>(device,resourceManager,*downSamplingRenderPasses[i],nowDownExtent,descSetLayout,pcPost);
			pbbDownSamplingRenderPasses.push_back(std::move(pbbDownSamplingRenderPass));

			VkExtent2D nowUpExtent = {bloomInputTextureSizes[N - i - 1].x / 2 , bloomInputTextureSizes[N - i - 1].y / 2 };
			auto pbbUpSamplingRenderPass = std::make_unique<PBBUpSamplingRenderPass>(device,resourceManager,*upSamplingRenderPasses[i],nowUpExtent,descSetLayout,pcPost);
			pbbUpSamplingRenderPasses.push_back(std::move(pbbUpSamplingRenderPass));
	}
	createRenderTarget();

	createBloomSizeBuffers();

	updateDescriptorSet();
	
}

void PBBloomPipelineBuilder::createDescriptorSetLayout()
{
	descSetBindings.clear();
	descSetLayout.reset();
	descPool.reset();
	upSamplingDescSets.clear();
	downSamplingDescSets.clear();



	descSetBindings.addBinding(PBBloomBindings::epbbloomInput,  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
									VK_SHADER_STAGE_VERTEX_BIT |VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR |  VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
	descSetBindings.addBinding(PBBloomBindings::epbInputSize,  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
								VK_SHADER_STAGE_VERTEX_BIT |VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR |  VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
	
	descSetLayout = descSetBindings.createLayout(device);
	descPool      = descSetBindings.createPool(device,2*N);

	for(int i = 0; i < N;i++)
	{

		auto upDescSet       = descPool->allocateDescriptorSet(*descSetLayout);
		upSamplingDescSets.push_back(std::move(upDescSet));
		auto downDescSet       = descPool->allocateDescriptorSet(*descSetLayout);
		downSamplingDescSets.push_back(std::move(downDescSet));
	}

}

void PBBloomPipelineBuilder::updateDescriptorSet()
{



	// downSampling
	for(int i = 0;i < N; i++)
	{
		std::vector<VkWriteDescriptorSet> writes;
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			if(i == 0)
			{
				imageInfo.imageView = gBufferRenderTarget->getImageViewByIndex(0).getHandle();
			}
			else 
			{
				imageInfo.imageView = renderTargets[i -1]->getImageViewByIndex(0).getHandle();
			}

			imageInfo.sampler = resourceManager.getClampToEdgeSampler().getHandle();
		
			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = downSamplingDescSets[i];
			write.dstBinding = PBBloomBindings::epbbloomInput;
			write.dstArrayElement = 0;
			write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write.descriptorCount = 1;
			write.pImageInfo = &imageInfo;
		
			writes.push_back(write);
		}

		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = bloomSizeBuffers[i]->getHandle();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(glm::vec2);
		
			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = downSamplingDescSets[i];
			write.dstBinding = PBBloomBindings::epbInputSize;
			write.dstArrayElement = 0;
			write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			write.descriptorCount = 1;
			write.pBufferInfo = &bufferInfo;
		
			writes.push_back(write);
		}

		vkUpdateDescriptorSets(device.getHandle(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
	}

	// upSampling
	for(int i = 0; i < N; i++)
	{
		std::vector<VkWriteDescriptorSet> writes;
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			if(i == N - 1)
			{
				imageInfo.imageView = renderTargets[i]->getImageViewByIndex(0).getHandle();
			}
			else 
			{
				imageInfo.imageView = renderTargets[i + 1]->getImageViewByIndex(0).getHandle();
			}
			
			imageInfo.sampler = resourceManager.getClampToEdgeSampler().getHandle();
				
			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = upSamplingDescSets[i];
			write.dstBinding = PBBloomBindings::epbbloomInput;
			write.dstArrayElement = 0;
			write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write.descriptorCount = 1;
			write.pImageInfo = &imageInfo;
				
			writes.push_back(write);
		}

		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = bloomSizeBuffers[i]->getHandle();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(glm::vec2);
				
			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = upSamplingDescSets[i];
			write.dstBinding = PBBloomBindings::epbInputSize;
			write.dstArrayElement = 0;
			write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			write.descriptorCount = 1;
			write.pBufferInfo = &bufferInfo;
				
			writes.push_back(write);
		}

		vkUpdateDescriptorSets(device.getHandle(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
	}





}

void PBBloomPipelineBuilder::draw(CommandBuffer& cmd)
{
	VkClearValue clearValue;
	clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };

	// downSampling
	for(int i = 0; i < N; i++)
	{
		cmd.beginRenderPass(*downSamplingRenderPasses[i],*downSamplingFramebuffers[i],{clearValue});
		pbbDownSamplingRenderPasses[i]->draw(cmd,{downSamplingDescSets[i]});
		cmd.endRenderPass();
	}

	// upSampling
	for(int i = 0; i < N; i++)
	{
		cmd.beginRenderPass(*upSamplingRenderPasses[i],*upSamplingFramebuffers[i],{clearValue});
		pbbUpSamplingRenderPasses[i]->draw(cmd,{upSamplingDescSets[i]});
		cmd.endRenderPass();
	}
}

void PBBloomPipelineBuilder::createRenderTarget()
{

	renderTargets.clear();
	downSamplingFramebuffers.clear();
	upSamplingFramebuffers.clear();


	std::vector<Image> images;
	for(int i = 0; i < N; i++)
	{
		VkExtent2D nowExtent = {bloomInputTextureSizes[i].x /2 , bloomInputTextureSizes[i].y /2 };
		std::unique_ptr<Image> bloomImage = std::make_unique<Image>(device,
						nowExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);

		images.push_back(std::move(*bloomImage));
		

		auto renderTarget = std::make_unique<RenderTarget>(std::move(images));

		renderTargets.push_back(std::move(renderTarget));

	    auto downSamplingFramebuffer = std::make_unique<FrameBuffer>(device,*renderTargets[i],*downSamplingRenderPasses[i]);
		downSamplingFramebuffers.push_back(std::move(downSamplingFramebuffer));

		auto upSamplingFramebuffer = std::make_unique<FrameBuffer>(device,*renderTargets[i],*upSamplingRenderPasses[i]);
		upSamplingFramebuffers.push_back(std::move(upSamplingFramebuffer));

		
		
	}

	std::reverse(upSamplingFramebuffers.begin(),upSamplingFramebuffers.end());




}

void PBBloomPipelineBuilder::createBloomSizeBuffers()
{

	bloomSizeBuffers.clear();
	for(int i = 0; i < N;i++)
	{
		std::vector<glm::vec2> tempVec(1,bloomInputTextureSizes[i]);
		auto bloomSizeBuffer = std::make_unique<Buffer>(device, tempVec,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		bloomSizeBuffers.push_back(std::move(bloomSizeBuffer));
	}
}

void mini::PBBloomPipelineBuilder::calculateBloomSizes()
{
	bloomInputTextureSizes.clear();
	auto originalExtent = gBufferRenderTarget->getExtent();
	for(int i = 0; i < N; i++)
	{
		bloomInputTextureSizes.push_back({originalExtent.width,originalExtent.height});
		originalExtent.width /= 2;
		originalExtent.height /= 2;
		

	}
}
