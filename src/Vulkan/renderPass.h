#pragma once
#include"Common/common.h"

namespace mini
{
class Device;

class RenderPass
{
public:
	RenderPass(Device &device, VkFormat swapchainFormat,VkImageLayout colorAttachmentFinalLayout=VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	~RenderPass();

	VkRenderPass getHandle() const;

private:
	VkRenderPass handle{ VK_NULL_HANDLE };

	Device& device;
};
}