#include"commandPool.h"
#include"device.h"
#include"commandBuffer.h"

namespace mini
{
CommandPool::CommandPool(Device& device, VkCommandPoolCreateFlags flag)
    :device(device)
{
    VkCommandPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    poolInfo.flags = flag| VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = device.getGraphicQueue().getFamilyIndex();

    if (vkCreateCommandPool(device.getHandle(), &poolInfo, nullptr, &handle) != VK_SUCCESS) {
        throw Error("Failed to create comand pool!");
    }
}
CommandPool::~CommandPool()
{
    if (handle!=VK_NULL_HANDLE) {
        vkDestroyCommandPool(device.getHandle(), handle, nullptr);
    }
}
Device& CommandPool::getDevice() const
{
    return device;
}
VkCommandPool CommandPool::getHandle() const
{
    return handle;
}
std::unique_ptr<CommandBuffer> CommandPool::createCommandBuffer()
{
    std::unique_ptr<CommandBuffer> buffer = std::make_unique<CommandBuffer>(*this);

    return std::move(buffer);
}

}