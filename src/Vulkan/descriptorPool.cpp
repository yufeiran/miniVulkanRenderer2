#include"descriptorPool.h"
#include"device.h"
#include"descriptorSet.h"
#include"descriptorSetLayout.h"

namespace mini
{



DescriptorPool::DescriptorPool(Device& device):device(device)
{
	VkDescriptorPoolSize poolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
	poolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
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

DescriptorSet& DescriptorPool::allocate(DescriptorSetLayout& descriptorSetLayout, const BindingMap<VkDescriptorBufferInfo>& bufferInfo)
{

	std::unique_ptr<DescriptorSet> set = std::make_unique<DescriptorSet>(*this, descriptorSetLayout, bufferInfo);
	descriptorSets.push_back(std::move(set));
	return *descriptorSets.back();
}

std::vector<std::unique_ptr<DescriptorSet>>& DescriptorPool::getDescriptorSets()
{
	return descriptorSets;
}

}