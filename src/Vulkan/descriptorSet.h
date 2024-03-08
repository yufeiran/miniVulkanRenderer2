#pragma once

#include"Common/common.h"

namespace mini
{
class DescriptorPool;
class DescriptorSetLayout;
class Device;

class DescriptorSet
{
public:
	DescriptorSet(DescriptorPool& descriptorPool, DescriptorSetLayout& descriptorSetLayout,
		const BindingMap<VkDescriptorBufferInfo>& bufferInfo = {},
		const BindingMap<VkDescriptorImageInfo>& imageInfo = {});
	~DescriptorSet();

	DescriptorSet(const DescriptorSet&des);

	VkDescriptorSet getHandle() const;
private:
	VkDescriptorSet handle{ VK_NULL_HANDLE };
	DescriptorPool& descriptorPool;
	DescriptorSetLayout& descriptorSetLayout;
	Device& device;

};

}