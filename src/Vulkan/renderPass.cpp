#include"renderPass.h"
#include"Vulkan/device.h"

namespace mini
{
RenderPass::RenderPass(Device& device, VkFormat swapchainFormat,VkImageLayout colorAttachmentFinalLayout,const std::vector<SubpassInfo>& subpasses)
	:device(device)
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchainFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = colorAttachmentFinalLayout;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = device.getPhysicalDevice().findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::vector<VkSubpassDescription> subpassDescriptions;

	for(int i=0; i <subpasses.size();i++)
	{
			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;
			subpass.pDepthStencilAttachment = &depthAttachmentRef;
			subpassDescriptions.push_back(subpass);
	}


	if(subpasses.size() == 0)
	{
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpassDescriptions.push_back(subpass);

	}

	//VkSubpassDescription subpass{};
	//subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	//subpass.colorAttachmentCount = 1;
	//subpass.pColorAttachments = &colorAttachmentRef;
	//subpass.pDepthStencilAttachment = &depthAttachmentRef;

	//VkSubpassDependency dependency{};
	//dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	//dependency.dstSubpass = 0;
	//dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	//dependency.srcAccessMask = 0;
	//dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	//dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::vector<VkSubpassDependency> dependencies;

	for(const auto& subpass:subpasses)
	{
		for(int i = 0; i < subpass.dependencies.size(); i++)
		{
			dependencies.push_back(subpass.dependencies[i]);
		}
	
	}


	std::vector<VkAttachmentDescription> attachments = { colorAttachment ,depthAttachment };



	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = subpassDescriptions.size();
	renderPassInfo.pSubpasses = subpassDescriptions.data();
	renderPassInfo.dependencyCount = dependencies.size();
	renderPassInfo.pDependencies = dependencies.data();

	if (vkCreateRenderPass(device.getHandle(), &renderPassInfo, nullptr, &handle) != VK_SUCCESS) {
		throw Error("Failed to create render pass");
	}

}
RenderPass::~RenderPass()
{
	if (handle != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(device.getHandle(), handle, nullptr);
	}
}
VkRenderPass RenderPass::getHandle() const
{
	return handle;
}
}