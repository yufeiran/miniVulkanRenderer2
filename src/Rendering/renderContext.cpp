#include"renderContext.h"
#include"Platform/glfwWindow.h"
#include"Vulkan/device.h"
#include"Vulkan/swapchain.h"
#include"Vulkan/image.h"
#include"Vulkan/imageView.h"
#include"renderFrame.h"	
#include"renderTarget.h"

namespace mini
{

VkFormat RenderContext::DEFAULT_VK_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

RenderContext::RenderContext(Device& device, VkSurfaceKHR surface, const GlfwWindow& window)
	:device(device),window(window),queue(device.getGraphicQueue()),surfaceExtent(window.getExtent())
{
	if (surface != VK_NULL_HANDLE)
	{
		swapchain = std::make_unique<Swapchain>(device, surface, surfaceExtent);
	}
}

void RenderContext::prepare(RenderTarget::CreateFunc createRenderTargetFunc)
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
			frames.emplace_back(std::make_unique<RenderFrame>(device, std::move(renderTarget)));

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

}
