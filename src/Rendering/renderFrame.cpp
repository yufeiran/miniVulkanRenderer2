#include "renderFrame.h"
#include"Vulkan/descriptorPool.h"
#include"Vulkan/descriptorSetLayout.h"
#include"Vulkan/descriptorSet.h"
#include"Vulkan/device.h"
#include"renderTarget.h"
#include"Vulkan/renderPass.h"
#include"Vulkan/framebuffer.h"
#include"Vulkan/semaphore.h"
#include"Vulkan/fence.h"
#include"Vulkan/commandBuffer.h"
#include"Vulkan/commandPool.h"



#include"../shaders/deviceDataStruct.h"
#include"Vulkan/buffer.h"
#include"ResourceManagement/ResourceManager.h"
#include"Vulkan/sampler.h"
#include"ResourceManagement/model.h"
#include"Vulkan/shaderInfo.h"
#include"ResourceManagement/model.h"
#include"Common/camera.h"


namespace mini
{
RenderFrame::RenderFrame(Device& device, ResourceManager& resourceManager, std::unique_ptr<RenderTarget>&& renderTarget, const RenderPass& renderPass,
	std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts, ShaderInfo& shaderInfo) :
	device(device), renderTarget(std::move(renderTarget)),descriptorSetLayouts(descriptorSetLayouts), resourceManager(resourceManager)
{
	frameBuffer = std::make_unique<FrameBuffer>(device, *this->renderTarget, renderPass);

	descriptorPool = std::make_unique<DescriptorPool>(device);

	createUniformBuffer();

	createDescriptorSets(resourceManager, shaderInfo);
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
	VkDeviceSize uniformBufferSize = sizeof(GlobalUniforms);
	uniformBuffers.resize(1);
	for (int i = 0; i < uniformBuffers.size(); i++)
	{
		uniformBuffers[i]= std::make_unique<Buffer>(device, uniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		uniformBuffers[i]->persistentMap(uniformBufferSize);
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i]->getHandle();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(GlobalUniforms);
		bufferInfos[0][i] = bufferInfo;
	}

}

void RenderFrame::updateUniformBuffer(Camera& c)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();

	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	auto extent = getExtent();

	glm::mat4 view = c.getViewMat();
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)extent.width / (float)extent.height, 0.1f, 1000.0f);
	proj[1][1] *= -1;
	GlobalUniforms ubo{};
	//ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.viewProj = proj * view;
	ubo.viewInverse = glm::inverse(view);
	ubo.projInverse = glm::inverse(proj);

	memcpy(uniformBuffers[0]->getMapAddress(), &ubo, sizeof(ubo));

}



void RenderFrame::createDescriptorSets(ResourceManager& resourceManager,ShaderInfo& shaderInfo)
{

	//为每个模型的每个shape都创建一个descriptorset

	auto& modelMap = resourceManager.getModelMap();
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
				imageInfo.imageView = resourceManager.getDefaultImageView().getHandle();
			}
			imageInfo.sampler = resourceManager.getDefaultSampler().getHandle();
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