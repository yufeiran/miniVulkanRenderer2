#include"renderPass.h"
#include"Vulkan/device.h"

namespace mini
{

RenderPass::RenderPass(Device& device, const std::vector<Attachment>& attachments, const std::vector<LoadStoreInfo>& loadStoreInfos, const std::vector<SubpassInfo>& subpasses)
	:device(device)
{
	//VkAttachmentDescription colorAttachment{};
	//colorAttachment.format = swapchainFormat;
	//colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	//colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	//colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	//colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//colorAttachment.finalLayout = colorAttachmentFinalLayout;

	//VkAttachmentReference colorAttachmentRef{};
	//colorAttachmentRef.attachment = 0;
	//colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//VkAttachmentDescription depthAttachment{};
	//depthAttachment.format = device.getPhysicalDevice().findDepthFormat();
	//depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	//depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//VkAttachmentReference depthAttachmentRef{};
	//depthAttachmentRef.attachment = 1;
	//depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::vector<VkAttachmentDescription> attachmentDescs{attachments.size()};

	for(size_t i =0; i< attachments.size(); i++)
	{
		attachmentDescs[i].format = attachments[i].format;
		attachmentDescs[i].samples = attachments[i].samples;

		attachmentDescs[i].loadOp = loadStoreInfos[i].loadOp;
		attachmentDescs[i].storeOp = loadStoreInfos[i].storeOp;

		attachmentDescs[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescs[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		attachmentDescs[i].initialLayout = attachments[i].initialLayout;
		attachmentDescs[i].finalLayout = attachments[i].finalLayout == VK_IMAGE_LAYOUT_UNDEFINED?
			isDepthStencilFormat(attachments[i].format)?
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		attachments[i].finalLayout;
	}

	std::vector<VkAttachmentReference> refs;


	for(size_t i = 0; i < attachmentDescs.size(); i++)
	{
		VkAttachmentReference ref;

		ref.attachment = i;

		if(isDepthStencilFormat(attachmentDescs[i].format))
		{
			ref.layout = attachmentDescs[i].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED ?
								VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:attachmentDescs[i].initialLayout;
		}
		else
		{
			ref.layout = attachmentDescs[i].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED ?
								VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:attachmentDescs[i].initialLayout;
		}
		refs.push_back(ref);
	}


	std::vector<VkSubpassDescription> subpassDescriptions;

	for(int i=0; i <subpasses.size();i++)
	{
			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			

			int colorIndex = subpasses[i].output[0];
			if(colorIndex!=-1)
			{
				VkAttachmentReference& colorAttachmentRef = refs[colorIndex];
				subpass.pColorAttachments = &colorAttachmentRef;
				subpass.colorAttachmentCount = 1;

			}
			else 
			{
				subpass.pColorAttachments = nullptr;
				subpass.colorAttachmentCount = 0;
			}


			int depthIndex = subpasses[i].output[1];
			if(depthIndex!=-1)
			{				
				VkAttachmentReference& depthAttachmentRef = refs[depthIndex];
				subpass.pDepthStencilAttachment = &depthAttachmentRef;
			}
			else 
			{
				subpass.pDepthStencilAttachment = nullptr;
			}
			subpassDescriptions.push_back(subpass);
	}


	if(subpasses.size() == 0)
	{
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &refs[0];
		subpass.pDepthStencilAttachment = &refs[1];
		subpassDescriptions.push_back(subpass);

	}


	std::vector<VkSubpassDependency> dependencies;

	for(const auto& subpass:subpasses)
	{
		for(int i = 0; i < subpass.dependencies.size(); i++)
		{
			dependencies.push_back(subpass.dependencies[i]);
		}
	
	}




	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachmentDescs.size();
	renderPassInfo.pAttachments = attachmentDescs.data();
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