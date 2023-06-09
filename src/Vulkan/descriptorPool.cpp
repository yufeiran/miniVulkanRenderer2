#include"descriptorPool.h"
#include"device.h"
#include"descriptorSet.h"
#include"descriptorSetLayout.h"

namespace mini
{



DescriptorPool::DescriptorPool(Device& device):device(device)
{
	std::array< VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1000;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1000;

	VkDescriptorPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1;

	if (vkCreateDescriptorPool(device.getHandle(), &poolInfo, nullptr, &handle) != VK_SUCCESS)
	{
		throw Error("Failed to create descriptor pool!");
	}
}

DescriptorPool::~DescriptorPool()
{
	if (handle != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(device.getHandle(), handle, nullptr);
	}
}

VkDescriptorPool DescriptorPool::getHandle()
{
	return handle;
}

Device& DescriptorPool::getDevice()
{
	return device;
}

DescriptorSet& DescriptorPool::allocate(DescriptorSetLayout& descriptorSetLayout, 
	const BindingMap<VkDescriptorBufferInfo>& bufferInfo,
	const BindingMap<VkDescriptorImageInfo>& imageInfo)
{

	std::unique_ptr<DescriptorSet> set = std::make_unique<DescriptorSet>(*this, descriptorSetLayout, bufferInfo, imageInfo);
	descriptorSets.push_back(std::move(set));
	return *descriptorSets.back();
}

std::vector<std::unique_ptr<DescriptorSet>>& DescriptorPool::getDescriptorSets()
{
	return descriptorSets;
}

}