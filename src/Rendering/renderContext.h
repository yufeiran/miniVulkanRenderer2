#pragma once
#include"Vulkan/vk_common.h"
#include"renderTarget.h"
#include"renderFrame.h"

namespace mini
{
class Device;
class GlfwWindow;
class RenderFrame;
class Queue;
class Swapchain;
class RenderTarget;
class Image;

/* RenderContext acts as a frame manager 
*  用来接管swapchain，负责创建renderFrame并且管理renderFrame的生命周期
*
*/
class RenderContext
{
public:
	static VkFormat DEFAULT_VK_FORMAT;


	RenderContext(Device& device, VkSurfaceKHR surface, const GlfwWindow& window);

	void prepare(RenderTarget::CreateFunc createRenderTargetFunc= RenderTarget::DEFAULT_CREATE_FUNC);

	VkExtent2D const& getSurfaceExtent() const;

	VkFormat getFormat() const;

private:
	Device& device;

	const GlfwWindow& window;

	const Queue& queue;

	std::unique_ptr<Swapchain> swapchain;

	std::vector<std::unique_ptr<RenderFrame>> frames;

	VkExtent2D surfaceExtent;
	

};
}