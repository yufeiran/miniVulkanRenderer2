#include "renderFrame.h"
#include"Vulkan/device.h"
#include"renderTarget.h"
#include"renderPass.h"
#include"Vulkan/framebuffer.h"
#include"Vulkan/semaphore.h"
#include"Vulkan/fence.h"
#include"Vulkan/commandBuffer.h"
#include"Vulkan/commandPool.h"



namespace mini
{
RenderFrame::RenderFrame(Device& device, std::unique_ptr<RenderTarget>&& renderTarget, const RenderPass& renderPass) :
	device(device), renderTarget(std::move(renderTarget))
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
}