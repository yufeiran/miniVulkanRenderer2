#include <optional>

#include"renderPass.h"
#include"Vulkan/device.h"


namespace mini
{

	RenderPass::RenderPass(Device& device, const std::vector<Attachment>& attachments, const std::vector<LoadStoreInfo>& loadStoreInfos, const std::vector<SubpassInfo>& subpasses)
		:device(device)
	{
		

		std::vector<VkAttachmentDescription> attachmentDescs{ attachments.size() };

		for (size_t i = 0; i < attachments.size(); i++)
		{
			attachmentDescs[i].format = attachments[i].format;
			attachmentDescs[i].samples = attachments[i].samples;

			attachmentDescs[i].loadOp = loadStoreInfos[i].loadOp;
			attachmentDescs[i].storeOp = loadStoreInfos[i].storeOp;

			attachmentDescs[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDescs[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			attachmentDescs[i].initialLayout = attachments[i].initialLayout;
			attachmentDescs[i].finalLayout = attachments[i].finalLayout == VK_IMAGE_LAYOUT_UNDEFINED ?
				isDepthStencilFormat(attachments[i].format) ?
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL :
				attachments[i].finalLayout;
		}

		std::vector<VkAttachmentReference> refs;


		for (size_t i = 0; i < attachmentDescs.size(); i++)
		{
			VkAttachmentReference ref;

			ref.attachment = i;

			if (isDepthStencilFormat(attachmentDescs[i].format))
			{
				ref.layout = attachmentDescs[i].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED ?
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : attachmentDescs[i].initialLayout;
				if (attachmentDescs[i].finalLayout == VK_IMAGE_LAYOUT_GENERAL)
				{
					ref.layout = VK_IMAGE_LAYOUT_GENERAL;
				}
			}
			else
			{
				ref.layout = attachmentDescs[i].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED ?
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : attachmentDescs[i].initialLayout;
			}
			refs.push_back(ref);
		}


		std::vector<VkSubpassDescription> subpassDescriptions;

		std::vector<std::vector<VkAttachmentReference>> subpassColorRefs{ subpasses.size() };
		std::vector<std::optional<VkAttachmentReference>> subpassDepthRefs{ subpasses.size() };

		std::vector<std::vector<VkAttachmentReference>> subpassInputRefs{ subpasses.size() };

		int subpassIndex = 0;
		for (auto& subpass : subpasses)
		{
			// for every subpass get the color and depth attachment

			std::vector<VkAttachmentReference>& colorRefs = subpassColorRefs[subpassIndex];
			std::optional<VkAttachmentReference>& depthRefs = subpassDepthRefs[subpassIndex];
			std::vector<VkAttachmentReference>& inputRef = subpassInputRefs[subpassIndex];

			for (auto index : subpass.input)
			{
				if (index == -1)
				{
					continue;
				}
				auto ref = refs[index];
				ref.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ;
				inputRef.push_back(ref);
			}

			for (auto index : subpass.output)
			{
				if (index == -1)
				{
					continue;
				}
				auto ref = refs[index];
				if (isDepthStencilFormat(attachmentDescs[index].format))
				{
					depthRefs = ref;
				}
				else
				{
					colorRefs.push_back(ref);
				}
			}
			VkSubpassDescription subpass1{};
			subpass1.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

			if (inputRef.size() > 0)
			{
				subpass1.inputAttachmentCount = inputRef.size();
				subpass1.pInputAttachments = inputRef.data();
			}
			else
			{
				subpass1.inputAttachmentCount = 0;
				subpass1.pInputAttachments = nullptr;
			}

			if (colorRefs.size() > 0)
			{
				subpass1.colorAttachmentCount = colorRefs.size();
				subpass1.pColorAttachments = colorRefs.data();
			}
			else
			{
				subpass1.colorAttachmentCount = 0;
				subpass1.pColorAttachments = nullptr;
			}

			if (depthRefs.has_value())
			{
				subpass1.pDepthStencilAttachment = &depthRefs.value();
			}
			else {
				subpass1.pDepthStencilAttachment = nullptr;
			}
			subpassDescriptions.push_back(subpass1);
			subpassIndex++;

		}



		if (subpasses.size() == 0)
		{
			VkSubpassDescription subpass1{};
			subpass1.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass1.colorAttachmentCount = 1;
			subpass1.pColorAttachments = &refs[0];
			subpass1.pDepthStencilAttachment = &refs[1];
			subpassDescriptions.push_back(subpass1);

		}


		std::vector<VkSubpassDependency> dependencies;

		for (const auto& subpass : subpasses)
		{
			for (int i = 0; i < subpass.dependencies.size(); i++)
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