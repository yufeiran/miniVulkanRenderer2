#pragma once
#include"vk_common.h"

namespace mini
{
class Device;
class Buffer;

class DeviceMemory
{
public:
	DeviceMemory(Device& device, Buffer& buffer, VkMemoryPropertyFlags properties);
	~DeviceMemory();

	VkDeviceMemory getHandle() const;
private:
	Device& device;

	VkDeviceMemory handle{ VK_NULL_HANDLE };

};
}