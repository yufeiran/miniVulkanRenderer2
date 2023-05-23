#include"queue.h"
#include"device.h"


namespace mini
{
    Queue::Queue(Device& device, uint32_t familyIndex, VkQueueFamilyProperties properties, VkBool32 canPresent, uint32_t index)
        :device(device),
        familyIndex(familyIndex),
        index(index),
        canPresent(canPresent),
        properties(properties)
    {
        vkGetDeviceQueue(device.getHandle(), familyIndex, index, &handle);
    }
    Queue::Queue(Queue&& other)
        :device(other.device),
        handle(other.handle),
        familyIndex(other.familyIndex),
        index(other.index),
        canPresent(other.canPresent),
        properties(other.properties)
    {
        other.handle = VK_NULL_HANDLE;
        other.familyIndex = {};
        other.properties = {};
        other.canPresent = VK_FALSE;
        other.index = 0;
    }
    const Device& Queue::getDevice() const
    {
        return device;
    }
    VkQueue Queue::getHandle() const
    {
        return handle;
    }
    uint32_t Queue::getFamilyIndex() const
    {
        return familyIndex;
    }
    uint32_t Queue::getIndex() const
    {
        return index;
    }
    const VkQueueFamilyProperties& Queue::getProperties() const
    {
        return properties;
    }
    VkBool32 Queue::supportPresent()
    {
        return canPresent;
    }
    VkResult Queue::submit(const std::vector<VkSubmitInfo>& submitInfos, VkFence fence) const
    {
        return vkQueueSubmit(handle, submitInfos.size(), submitInfos.data(), fence);;
    }
    VkResult Queue::submit(const CommandBuffer& commandBuffer, VkFence fence) const
    {
        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.commandBufferCount = 1;
        return VkResult();
    }
    VkResult Queue::present(const VkPresentInfoKHR& presentInfos) const
    {
        return VkResult();
    }
    VkResult Queue::waitIdle() const
    {
        return VkResult();
    }
}