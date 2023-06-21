#include "renderFrame.h"
#include"Vulkan/descriptorPool.h"
#include"Vulkan/descriptorSetLayout.h"
#include"Vulkan/descriptorSet.h"
#include"Vulkan/device.h"
#include"renderTarget.h"
#include"renderPass.h"
#include"Vulkan/framebuffer.h"
#include"Vulkan/semaphore.h"
#include"Vulkan/fence.h"
#include"Vulkan/commandBuffer.h"
#include"Vulkan/commandPool.h"



#include"Common/deviceDataStruct.h"
#include"Vulkan/buffer.h"
#include"ResourceManagement/resourceManagement.h"
#include"Vulkan/sampler.h"
#include"ResourceManagement/model.h"
#include"Vulkan/shaderInfo.h"
#include"ResourceManagement/model.h"



namespace mini
{
RenderFrame::RenderFrame(Device& device, ResourceManagement& resourceManagement, std::unique_ptr<RenderTarget>&& renderTarget, const RenderPass& renderPass,
	std::vector<std::unique_ptr<DescriptorSetLayout>>& descriptorSetLayouts, ShaderInfo& shaderInfo) :
	device(device), renderTarget(std::move(renderTarget)),descriptorSetLayouts(descriptorSetLayouts), resourceManagement(resourceManagement)
{
	frameBuffer = std::make_unique<FrameBuffer>(device, *this->renderTarget, renderPass);

	descriptorPool = std::make_unique<DescriptorPool>(device);

	createUniformBuffer();

	//setImageInfos();

	createDescriptorSets(resourceManagement, shaderInfo);
}

Device& RenderFrame::getDevice()
{
	return device;
}

FrameBuffer& RenderFrame::getFrameBuffer()
{
	return *frameBuffer;
}

void RenderFrame::reset()
{

}

void RenderFrame::createUniformBuffer()
{
	VkDeviceSize uniformBufferSize = sizeof(UniformBufferObject);
	uniformBuffers.resize(1);
	for (int i = 0; i < uniformBuffers.size(); i++)
	{
		uniformBuffers[i]= std::make_unique<Buffer>(device, uniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		uniformBuffers[i]->persistentMap(uniformBufferSize);
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i]->getHandle();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);
		bufferInfos[0][i] = bufferInfo;
	}

}

void RenderFrame::setImageInfos()
{

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = resourceManagement.getImageViewByName("yamato").getHandle();
	imageInfo.sampler = resourceManagement.getDefaultSampler().getHandle();

	imageInfos[0][1] = imageInfo;
}

void RenderFrame::updateUniformBuffer()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();

	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	auto extent = getExtent();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), (float)extent.width / (float)extent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;
	memcpy(uniformBuffers[0]->getMapAddress(), &ubo, sizeof(ubo));

}



void RenderFrame::createDescriptorSets(ResourceManagement& resourceManagement,ShaderInfo& shaderInfo)
{

	//为每个模型的每个shape都创建一个descriptorset

	auto& modelMap = resourceManagement.getModelMap();
	for (auto& m : modelMap)
	{
		auto& shapeMap = m.second->getShapeMap();
		for (auto& s : shapeMap)
		{
			//在shaderInfo里找对应纹理的槽位
			auto diffusePos = shaderInfo.findTexturePos(DIFFUSE);


			auto& textureInfo= s.second->getTextureInfos();

			auto diffuseTextureInfo = s.second->findTextureInfo(DIFFUSE);
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			//如果shape中有diffuse贴图就用模型中的贴图
			if (diffuseTextureInfo.type != NONE)
			{
				imageInfo.imageView = m.second->getImageViewByName(diffuseTextureInfo.name).getHandle();
			}
			//如果没有，就用默认全黑贴图
			else {
				imageInfo.imageView = resourceManagement.getDefaultImageView().getHandle();
			}
			imageInfo.sampler = resourceManagement.getDefaultSampler().getHandle();
			imageInfos[diffusePos.first][diffusePos.second] = imageInfo;
			auto& descripotrSet = descriptorPool->allocate(*descriptorSetLayouts[0], bufferInfos, imageInfos);

			descriptorSetMap[m.second->getName()][s.second->getName()] = { descripotrSet.getHandle() };
			
		}

	}

	

}

const VkExtent2D RenderFrame::getExtent() const
{
	return renderTarget->getExtent();
}

std::vector<VkDescriptorSet>& RenderFrame::getDescriptorSet(Model& m, Shape& s)
{
	return descriptorSetMap[m.getName()][s.getName()];
}

}