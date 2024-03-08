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

	std::vector<VkWriteDescriptorSet> descriptorWrites;
	int descriptorWritesSize = bufferInfo.size()+ imageInfo.size();
	descriptorWrites.resize(descriptorWritesSize);

	int nowWriteIndex=0;
	for(const auto& setInfo:bufferInfo)
	{
		for(const auto& bindingInfo:setInfo.second)
		{
			descriptorWrites[nowWriteIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[nowWriteIndex].dstSet = handle;
			descriptorWrites[nowWriteIndex].dstBinding = bindingInfo.first;
			descriptorWrites[nowWriteIndex].dstArrayElement = 0;
			descriptorWrites[nowWriteIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[nowWriteIndex].descriptorCount = 1;
			descriptorWrites[nowWriteIndex].pBufferInfo = &bindingInfo.second;
			descriptorWrites[nowWriteIndex].pImageInfo = nullptr;
			descriptorWrites[nowWriteIndex].pTexelBufferView = nullptr;
			nowWriteIndex++;
		}
	}

	for(const auto& setInfo:imageInfo)
	{
		for(const auto& bindingInfo:setInfo.second)
		{
			descriptorWrites[nowWriteIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[nowWriteIndex].dstSet = handle;
			descriptorWrites[nowWriteIndex].dstBinding =  bindingInfo.first;
			descriptorWrites[nowWriteIndex].dstArrayElement = 0;
			descriptorWrites[nowWriteIndex].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[nowWriteIndex].descriptorCount = 1;
			descriptorWrites[nowWriteIndex].pBufferInfo = nullptr;
			descriptorWrites[nowWriteIndex].pImageInfo = &bindingInfo.second;
			descriptorWrites[nowWriteIndex].pTexelBufferView = nullptr;
			nowWriteIndex++;
		}
	}
	


	vkUpdateDescriptorSets(device.getHandle(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);

}

DescriptorSet::~DescriptorSet()
{

}

DescriptorSet::DescriptorSet(const DescriptorSet& des):
	descriptorPool(des.descriptorPool),
	descriptorSetLayout(des.descriptorSetLayout),
	device(des.device),
	handle(des.handle)
{
}

VkDescriptorSet DescriptorSet::getHandle() const
{
	return handle;
}

}