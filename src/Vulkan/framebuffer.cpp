#include "framebuffer.h"
#include"device.h"
#include"Rendering/renderTarget.h"
#include"Rendering/renderPass.h"
#include"imageView.h"

namespace mini
{
Framebuffer::Framebuffer(Device& device, RenderTarget& renderTarget, RenderPass& renderPass)
	:device(device)
{

	extent = renderTarget.getExtent();

	std::vector<VkImageView> attachments;

	for (auto& view : renderTarget.getViews())
	{
		attachments.emplace_back(view.getHandle());
	}

	VkFramebufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	createInfo.renderPass = renderPass.getHandle();
	createInfo.attachmentCount = attachments.size();
	createInfo.pAttachments = attachments.data();
	createInfo.width = extent.width;
	createInfo.height = extent.height;
	createInfo.layers = 1;

	if (vkCreateFramebuffer(device.getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
		throw Error("failed to create framebuffer!");
	}

}

Framebuffer::~Framebuffer()
{
	if (handle != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(device.getHandle(), handle, nullptr);
	}
}

VkFramebuffer Framebuffer::getHandle() const
{
	return handle;
}

}

