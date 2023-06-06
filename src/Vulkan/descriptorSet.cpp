#include"descriptorSet.h"
#include"descriptorPool.h"
#include"descriptorSetLayout.h"
#include"device.h"

namespace mini
{

DescriptorSet::DescriptorSet(DescriptorPool& descriptorPool, DescriptorSetLayout& descriptorSetLayout, const BindingMap<VkDescriptorBufferInfo>& bufferInfo)
	:descriptorPool(descriptorPool), descriptorSetLayout(descriptorSetLayout),device(descriptorPool.getDevice())
{
	VkDescriptorSetAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	allocInfo.descriptorPool = descriptorPool.getHandle();
	allocInfo.descriptorSetCount = 1;
	VkDescriptorSetLayout layout = descriptorSetLayout.getHandle();
	allocInfo.pSetLayouts = &(layout);

	if (vkAllocateDescriptorSets(descriptorPool.getDevice().getHandle(), &allocInfo, &handle) != VK_SUCCESS)
	{
		throw Error("Failed to allocate descriptor sets!");
	}

	VkWriteDescriptorSet descriptorWrite{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	descriptorWrite.dstSet = handle;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;

	descriptorWrite.pBufferInfo = &bufferInfo.at(0).at(0);
	descriptorWrite.pImageInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(device.getHandle(), 1, &descriptorWrite, 0, nullptr);




}

DescriptorSet::~DescriptorSet()
{

}

VkDescriptorSet DescriptorSet::getHandle()
{
	return handle;
}

}