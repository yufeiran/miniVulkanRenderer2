#include"Vulkan/descriptorSetBindings.h"


using namespace mini;

std::unique_ptr<DescriptorSetLayout> DescriptorSetBindings::createLayout(Device& device, VkDescriptorSetLayoutCreateFlags flags)
{
    return std::move(std::make_unique<DescriptorSetLayout>(device,bindings,flags));
}



// for binding find and add sutiable pool sizes
void DescriptorSetBindings::addRequiredPoolSizes(std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t numSets) const
{
    for(auto it = bindings.cbegin(); it != bindings.cend(); ++it)
    {
        if(it->descriptorCount==0)
        {
            continue;
        }

        bool found = false;

        for(auto itpool = poolSizes.begin(); itpool != poolSizes.end(); ++itpool)
        {
            if(itpool->type == it->descriptorType)
            {
                itpool->descriptorCount += it->descriptorCount * numSets;
                found = true;
                break;
            }
        }

        if(!found)
        {
            VkDescriptorPoolSize poolSize{};
            poolSize.type             = it->descriptorType;
            poolSize.descriptorCount  = it->descriptorCount * numSets;
            poolSizes.push_back(poolSize);
        }

    }
}

VkWriteDescriptorSet DescriptorSetBindings::makeWrite(VkDescriptorSet dstSet, uint32_t dstBinding, uint32_t arrayElement) const
{
    VkWriteDescriptorSet writeSet = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    writeSet.descriptorType       = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    for(size_t i = 0; i < bindings.size(); i++)
    {
        if(bindings[i].binding == dstBinding)
        {
            writeSet.descriptorCount  = 1;
            writeSet.descriptorType   = bindings[i].descriptorType;
            writeSet.dstBinding       = dstBinding;
            writeSet.dstSet           = dstSet;
            writeSet.dstArrayElement  = arrayElement;
            return writeSet;
        }
    }
    assert(0 && "binding no found");

    return writeSet;
}

VkWriteDescriptorSet DescriptorSetBindings::makeWrite(VkDescriptorSet dstSet, uint32_t dstBinding, const VkWriteDescriptorSetAccelerationStructureKHR* pAccel, uint32_t arrayElement) const
{
    VkWriteDescriptorSet writeSet = makeWrite(dstSet,dstBinding,arrayElement);
    assert(writeSet.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR);

    writeSet.pNext = pAccel;
    return writeSet;
}

VkWriteDescriptorSet mini::DescriptorSetBindings::makeWrite(VkDescriptorSet dstSet, uint32_t dstBinding, const VkDescriptorImageInfo* pImageInfo, uint32_t arrayElement) const
{
    VkWriteDescriptorSet writeSet = makeWrite(dstSet,dstBinding,arrayElement);
    assert(writeSet.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER || writeSet.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
           || writeSet.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE || writeSet.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
           || writeSet.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);

    writeSet.pImageInfo = pImageInfo;

    return writeSet;
}



std::unique_ptr<DescriptorPool> DescriptorSetBindings::createPool(Device& device, uint32_t maxSets, VkDescriptorPoolCreateFlags flags) const
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    addRequiredPoolSizes(poolSizes,maxSets);


    return std::move(std::make_unique<DescriptorPool>(device,poolSizes,maxSets));
}
