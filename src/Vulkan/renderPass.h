#pragma once
#include "Common/common.h"
#include "Rendering/renderTarget.h"

namespace mini
{
class Device;

struct LoadStoreInfo
{
	VkAttachmentLoadOp loadOp{ VK_ATTACHMENT_LOAD_OP_CLEAR };
	VkAttachmentStoreOp storeOp{ VK_ATTACHMENT_STORE_OP_STORE };
};

struct SubpassInfo
{
	std::vector<uint32_t> output{};
	std::vector<uint32_t> input{};
	std::vector<VkSubpassDependency> dependencies{};
};

class RenderPass
{
public:
	//RenderPass(Device &device, 
	//	VkFormat swapchainFormat,
	//	VkImageLayout colorAttachmentFinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	//	const std::vector<SubpassInfo> &subpasses = {});
	RenderPass(Device &device, 
		const std::vector<Attachment> &attachments,
		const std::vector<LoadStoreInfo> &loadStoreInfos,
		const std::vector<SubpassInfo> &subpasses = {});

	~RenderPass();

	VkRenderPass getHandle() const;

private:
	VkRenderPass handle{ VK_NULL_HANDLE };

	Device& device;
};
}