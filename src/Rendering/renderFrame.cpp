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
#include"Vulkan/shaderInfo.h"
#include"Common/camera.h"


namespace mini
{
RenderFrame::RenderFrame(Device& device, ResourceManager& resourceManager, std::unique_ptr<RenderTarget>&& renderTarget, const RenderPass& renderPass,
	std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts, ShaderInfo& shaderInfo) :
	device(device), renderTarget(std::move(renderTarget)), resourceManager(resourceManager)
{
	frameBuffer = std::make_unique<FrameBuffer>(device, *this->renderTarget, renderPass);

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



const VkExtent2D RenderFrame::getExtent() const
{
	return renderTarget->getExtent();
}

}