#pragma once

#include"Common/common.h"

namespace mini
{
class Device;
class DescriptorSet;
class DescriptorSetLayout;

class DescriptorPool
{
public:
	DescriptorPool(Device& device);
	DescriptorPool(Device& device,std::vector< VkDescriptorPoolSize> poolSizes,uint32_t maxSets);
	~DescriptorPool();

	VkDescriptorPool getHandle();
	Device& getDevice();

	DescriptorSet& allocate(DescriptorSetLayout& descriptorSetLayout,
		const BindingMap<VkDescriptorBufferInfo>& bufferInfo = {},
		const BindingMap<VkDescriptorImageInfo>& imageInfo = {});

	VkDescriptorSet allocateDescriptorSet(DescriptorSetLayout& layout);

	std::vector<std::unique_ptr< DescriptorSet>>& getDescriptorSets();

private:
	VkDescriptorPool handle{ VK_NULL_HANDLE };
	Device& device;
	std::vector<std::unique_ptr< DescriptorSet>> descriptorSets;

};


}