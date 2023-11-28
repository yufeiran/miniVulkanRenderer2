#include"renderContext.h"
#include"Platform/GUIWindow.h"
#include"Vulkan/commandPool.h"
#include"Vulkan/commandBuffer.h"
#include"Vulkan/device.h"
#include"Vulkan/swapchain.h"
#include"Vulkan/image.h"
#include"Vulkan/imageView.h"
#include"Vulkan/semaphore.h"
#include"Vulkan/fence.h"
#include"renderFrame.h"	
#include"renderTarget.h"
#include"Vulkan/descriptorPool.h"
#include"Vulkan/descriptorSet.h"
#include"Vulkan/descriptorSetLayout.h"
#include"Vulkan/buffer.h"
#include"Vulkan/shaderInfo.h"



namespace mini
{

VkFormat RenderContext::DEFAULT_VK_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

RenderContext::RenderContext(Device& device, VkSurfaceKHR surface, const GUIWindow& window)
	:device(device),window(window),queue(device.getGraphicQueue()),surfaceExtent(window.getExtent())
{
	if (surface != VK_NULL_HANDLE)
	{
		swapchain = std::make_unique<Swapchain>(device, surface, surfaceExtent);
	}
}

void RenderContext::prepare(const RenderPass& renderPass, 
	ResourceManager& resourceManager,
	std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts,
	ShaderInfo& shaderInfo,
	RenderTarget::CreateFunc createRenderTargetFunc)
{
	device.waitIdle();

	if (swapchain)
	{
		swapchain->create();

		surfaceExtent = swapchain->getExtent();

		for (auto& imageHandle : swapchain->getImages()) {
			auto swapchainImage = Image(device,
				imageHandle,
				surfaceExtent,
				swapchain->getImageFormat(),
				swapchain->getImageUsage());

			auto renderTarget = createRenderTargetFunc(std::move( swapchainImage));
			frames.emplace_back(std::make_unique<RenderFrame>(device, resourceManager, std::move(renderTarget), renderPass,
				descriptorSetLayouts,shaderInfo));
		}

		commandPool = std::make_unique<CommandPool>(device);

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{

			imageAvailableSemaphores.emplace_back(std::make_unique<Semaphore>(device));
			renderFinishedSemaphores.emplace_back(std::make_unique<Semaphore>(device));

			inFlightFences.emplace_back(std::make_unique<Fence>(device, VK_FENCE_CREATE_SIGNALED_BIT));

			commandBuffers.emplace_back(std::move(commandPool->createCommandBuffer()));
		}

		
	}

}

VkExtent2D const& RenderContext::getSurfaceExtent() const
{
	return surfaceExtent;
}

VkFormat RenderContext::getFormat() const
{
	VkFormat format = DEFAULT_VK_FORMAT;
	if (swapchain)
	{
		format = swapchain->getImageFormat();
	}
	return format;
}

RenderFrame& RenderContext::getActiveFrame()
{
	assert(frameActive && "Frame is not active, call begin_frame");
	assert(activeFrameIndex < frames.size());
	return *frames[activeFrameIndex];
}

VkResult RenderContext::beginFrame()
{
	assert(!frameActive && "Frame is still active, call endFrame");
	assert(activeFrameIndex < frames.size());


	auto& imageAvailableSemaphore = imageAvailableSemaphores[currentFrames];
	auto& inFlightFence = inFlightFences[currentFrames];


	inFlightFence->wait();
	inFlightFence->reset();

	VkResult result=VK_SUCCESS;
	if (swapchain)
	{
		result = swapchain->acquireNextImage(activeFrameIndex, imageAvailableSemaphore->getHandle(), VK_NULL_HANDLE);

	}
	//Log("now active frame index is " + toString(activeFrameIndex));

	frameActive = true;



	waitFrame();

	inFlightFence->reset();
	return result;

}

void RenderContext::submit(const Queue& queue, const CommandBuffer* cmd)
{
	VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };

	auto& imageAvailableSemaphore = imageAvailableSemaphores[currentFrames];
	auto& renderFinishedSemaphore = renderFinishedSemaphores[currentFrames];
	auto& inFlightFence = inFlightFences[currentFrames];




	VkCommandBuffer cmdBuffer =commandBuffers[currentFrames]->getHandle();

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore->getHandle()};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;

	VkSemaphore singnalSemaphores[] = { renderFinishedSemaphore->getHandle()};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = singnalSemaphores;

	VkResult result = vkQueueSubmit(device.getGraphicQueue().getHandle(), 1, &submitInfo, inFlightFence->getHandle());
	if (result != VK_SUCCESS) {
		throw Error("Failed to submit draw command buffer! result:"+ std::to_string(result));
	}

}


void RenderContext::endFrame()
{
	auto& imageAvailableSemaphore = imageAvailableSemaphores[currentFrames];
	auto& renderFinishedSemaphore = renderFinishedSemaphores[currentFrames];
	auto& inFlightFence = inFlightFences[currentFrames];

	assert(frameActive && "Frame is not active, call beginFrame");
	if (swapchain)
	{
		VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore->getHandle() };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		VkSwapchainKHR swapChains[] = { swapchain->getHandle() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &activeFrameIndex;

		presentInfo.pResults = nullptr;

		vkQueuePresentKHR(device.getPresentQueue().getHandle(), &presentInfo);
	}
	frameActive = false;

	currentFrames = (currentFrames + 1) % MAX_FRAMES_IN_FLIGHT;
}

void RenderContext::waitFrame()
{
	RenderFrame& frame = getActiveFrame();
	frame.reset();
}

CommandBuffer& RenderContext::getCurrentCommandBuffer()
{
	return *commandBuffers[currentFrames];
}

uint32_t RenderContext::getCurrentFrames()
{
	return currentFrames;
}



}
