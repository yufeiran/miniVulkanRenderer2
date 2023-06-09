#pragma once

#include"vk_common.h"

namespace mini
{
class Device;
class DescriptorSet;
class DescriptorSetLayout;

class DescriptorPool
{
public:
	DescriptorPool(Device& device);
	~DescriptorPool();

	VkDescriptorPool getHandle();
	Device& getDevice();

	DescriptorSet& allocate(DescriptorSetLayout& descriptorSetLayout,
		const BindingMap<VkDescriptorBufferInfo>& bufferInfo = {},
		const BindingMap<VkDescriptorImageInfo>& imageInfo = {});
	std::vector<std::unique_ptr< DescriptorSet>>& getDescriptorSets();

private:
	VkDescriptorPool handle{ VK_NULL_HANDLE };
	Device& device;
	std::vector<std::unique_ptr< DescriptorSet>> descriptorSets;

};


}