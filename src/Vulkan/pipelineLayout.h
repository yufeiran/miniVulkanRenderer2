#pragma once
#include "Common/common.h"

namespace mini
{
class DescriptorSetLayout;
class Device;

class PipelineLayout
{
public:
	PipelineLayout(Device& device,std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts, std::vector<VkPushConstantRange> pushConstants );
	~PipelineLayout();

	VkPipelineLayout getHandle() const {return handle;}
private:
	VkPipelineLayout handle{VK_NULL_HANDLE};
	Device &device;

};
}
