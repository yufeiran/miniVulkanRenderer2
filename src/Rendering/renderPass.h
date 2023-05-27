#pragma once
#include"Vulkan/vk_common.h"

namespace mini
{
class Device;

class RenderPass
{
public:
	RenderPass(Device &device, VkFormat swapchainFormat);

	~RenderPass();

	VkRenderPass getHandle() const;

private:
	VkRenderPass handle{ VK_NULL_HANDLE };

	Device& device;
};
}