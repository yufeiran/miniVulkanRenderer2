#include"descriptorPool.h"
#include"device.h"
#include"descriptorSet.h"
#include"descriptorSetLayout.h"

namespace mini
{

// default create fun
DescriptorPool::DescriptorPool(Device& device):device(device)
{
	std::vector< VkDescriptorPoolSize> poolSizes{};
	poolSizes.resize(3);
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1000;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1000;
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	poolSizes[2].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1000;

	if (vkCreateDescriptorPool(device.getHandle(), &poolInfo, nullptr, &handle) != VK_SUCCESS)
	{
		throw Error("Failed to create descriptor pool!");
	}

}

DescriptorPool::DescriptorPool(Device& device,std::vector< VkDescriptorPoolSize> poolSizes,uint32_t maxSets):device(device)
{
	
	VkDescriptorPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = maxSets;

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

VkDescriptorSet DescriptorPool::allocateDescriptorSet(DescriptorSetLayout& layout)
{
	VkDescriptorSetLayout vkLayout = layout.getHandle();
	VkDescriptorSetAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
	allocInfo.descriptorPool              = handle;
	allocInfo.descriptorSetCount          = 1;
	allocInfo.pSetLayouts                 = &vkLayout;

	VkResult result;
	VkDescriptorSet set;
	result = vkAllocateDescriptorSets(device.getHandle(),&allocInfo,&set);

	if(result != VK_SUCCESS)
	{
		Log("vkAllocateDescriptorSets Failed",LOG_TYPE::ERROR_TYPE);
	}

	return set;
}

std::vector<std::unique_ptr<DescriptorSet>>& DescriptorPool::getDescriptorSets()
{
	return descriptorSets;
}

}