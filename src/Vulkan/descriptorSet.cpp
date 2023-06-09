#include"descriptorSet.h"
#include"descriptorPool.h"
#include"descriptorSetLayout.h"
#include"device.h"

namespace mini
{

DescriptorSet::DescriptorSet(DescriptorPool& descriptorPool, DescriptorSetLayout& descriptorSetLayout
	, const BindingMap<VkDescriptorBufferInfo>& bufferInfo,
	const BindingMap<VkDescriptorImageInfo>& imageInfo)
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

	std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = handle;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;

	descriptorWrites[0].pBufferInfo = &bufferInfo.at(0).at(0);
	descriptorWrites[0].pImageInfo = nullptr;
	descriptorWrites[0].pTexelBufferView = nullptr;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = handle;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;

	descriptorWrites[1].pBufferInfo = nullptr;
	descriptorWrites[1].pImageInfo = &imageInfo.at(0).at(1);
	descriptorWrites[1].pTexelBufferView = nullptr;



	vkUpdateDescriptorSets(device.getHandle(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);




}

DescriptorSet::~DescriptorSet()
{

}

VkDescriptorSet DescriptorSet::getHandle()
{
	return handle;
}

}