#pragma once 

#include"Common/common.h"
#include"Vulkan/device.h"
#include"Vulkan/descriptorPool.h"
#include"Vulkan/descriptorSet.h"
#include"Vulkan/descriptorSetLayout.h"

namespace mini
{

class DescriptorSetBindings
{
public:

	DescriptorSetBindings()=default;

	DescriptorSetBindings(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
		:bindings(bindings)
	{
	}

	void addBinding(uint32_t             binding,
					VkDescriptorType     type,
					uint32_t             count,
					VkShaderStageFlags   stageFlags,
					const VkSampler*     pImmutableSampler = nullptr)
	{
		bindings.push_back({binding,type,count,stageFlags,pImmutableSampler});
	}

	void addBinding(const VkDescriptorSetLayoutBinding& layoutBinding){bindings.emplace_back(layoutBinding);}

	void setBinding(const std::vector<VkDescriptorSetLayoutBinding>& bindings){this->bindings = bindings;}

	void clear(){bindings.clear();}

	std::unique_ptr<DescriptorSetLayout> createLayout(Device&                          device,
													  VkDescriptorSetLayoutCreateFlags flags       = 0);

	std::unique_ptr<DescriptorPool> createPool(Device& device,uint32_t maxSets = 1, VkDescriptorPoolCreateFlags flags =0) const;

	void addRequiredPoolSizes(std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t numSets) const;

	VkWriteDescriptorSet makeWrite(VkDescriptorSet dstSet, uint32_t dstBinding, uint32_t arrayElement = 0) const;

	VkWriteDescriptorSet makeWrite(VkDescriptorSet                                         dstSet,
								   uint32_t                                                dstBinding,
		                           const VkWriteDescriptorSetAccelerationStructureKHR*     pAccel,
		                           uint32_t                                                arrayElement = 0) const;

	VkWriteDescriptorSet makeWrite(VkDescriptorSet                                         dstSet,
								   uint32_t                                                dstBinding,
								   const VkDescriptorImageInfo*                            pImageInfo,
								   uint32_t                                                arrayElement = 0) const;


private:
	std::vector<VkDescriptorSetLayoutBinding> bindings;

};


};