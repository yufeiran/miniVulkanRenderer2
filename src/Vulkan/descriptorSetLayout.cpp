#include"descriptorSetLayout.h"
#include"device.h"

namespace mini
{

DescriptorSetLayout::DescriptorSetLayout(Device& device, const std::vector< VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings):device(device)
{


	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = vkDescriptorSetLayoutBindings.size();
	layoutInfo.pBindings = vkDescriptorSetLayoutBindings.data();

	if (vkCreateDescriptorSetLayout(device.getHandle(), &layoutInfo, nullptr, &handle) != VK_SUCCESS)
	{
		throw Error("Failed to create descriptor set layout!");
	}

}

DescriptorSetLayout::~DescriptorSetLayout()
{
	if (handle != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(device.getHandle(), handle, nullptr);
	}
}

VkDescriptorSetLayout DescriptorSetLayout::getHandle() const
{
	return handle;
}

}