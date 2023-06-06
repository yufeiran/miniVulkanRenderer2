#pragma once 
#include"vk_common.h"

namespace mini
{
class Device;

class DescriptorSetLayout
{
public:
	DescriptorSetLayout(Device& device,const std::vector< VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings);
	~DescriptorSetLayout();

	VkDescriptorSetLayout getHandle() const;
private:
	VkDescriptorSetLayout handle{ VK_NULL_HANDLE };

	Device& device;

};
}