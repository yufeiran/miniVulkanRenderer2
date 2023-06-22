#pragma once
#include"Common/common.h"

namespace mini
{
class Device;
class Buffer;
class Image;

class DeviceMemory
{
public:
	DeviceMemory(Device& device, Buffer& buffer, VkMemoryPropertyFlags properties);
	DeviceMemory(Device& device, Image& image, VkMemoryPropertyFlags properties);
	~DeviceMemory();

	VkDeviceMemory getHandle() const;
private:
	Device& device;

	VkDeviceMemory handle{ VK_NULL_HANDLE };

};
}