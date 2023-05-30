#pragma once
#include"vk_common.h"

namespace mini
{
class Device;
class CommandBuffer;

class CommandPool
{
public:
	CommandPool(Device& device, VkCommandPoolCreateFlags flag = {});

	~CommandPool();

	Device& getDevice() const;

	VkCommandPool getHandle() const;

	std::unique_ptr<CommandBuffer>createCommandBuffer();
private:
	Device& device;

	VkCommandPool handle{ VK_NULL_HANDLE };
};

}