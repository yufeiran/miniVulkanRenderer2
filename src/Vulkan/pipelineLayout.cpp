#include "pipelineLayout.h"
#include"Vulkan/device.h"
#include"Vulkan/descriptorSetLayout.h"


using namespace mini;
PipelineLayout::PipelineLayout(Device& device, std::vector<std::unique_ptr<DescriptorSetLayout>>& descriptorSetLayouts, 
	std::vector<VkPushConstantRange> pushConstants):device(device)
{

	std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts;

	for (const auto& it : descriptorSetLayouts)
	{
		vkDescriptorSetLayouts.push_back(it->getHandle());
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = vkDescriptorSetLayouts.size();
	pipelineLayoutInfo.pSetLayouts = vkDescriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = pushConstants.size();
	pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();

	if (vkCreatePipelineLayout(device.getHandle(), &pipelineLayoutInfo, nullptr, &handle) != VK_SUCCESS) {
		throw Error("Failed to crate pipeline layout");
	}
	Log("Pipeline layout created!");
}

mini::PipelineLayout::~PipelineLayout()
{
	if(handle!=VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(device.getHandle(),handle,nullptr);
	}
}
