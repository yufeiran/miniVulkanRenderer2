#pragma once 
#include"Common/common.h"

namespace mini
{
class Device;

class DescriptorSetLayout
{
public:
	DescriptorSetLayout(Device& device,
		const std::vector< VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings,VkDescriptorSetLayoutCreateFlags flags=0);
	~DescriptorSetLayout();

	VkDescriptorSetLayout getHandle() const;
private:
	VkDescriptorSetLayout handle{ VK_NULL_HANDLE };

	Device& device;

};
}