#pragma once
#include"Common/common.h"

namespace mini
{
class Device;

struct SubpassInfo
{
	std::vector<uint32_t> output{};
	std::vector<uint32_t> input{};
	std::vector<VkSubpassDependency> dependencies{};
};

class RenderPass
{
public:
	RenderPass(Device &device, 
		VkFormat swapchainFormat,
		VkImageLayout colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		const SubpassInfo &subpasses = {});

	~RenderPass();

	VkRenderPass getHandle() const;

private:
	VkRenderPass handle{ VK_NULL_HANDLE };

	Device& device;
};
}