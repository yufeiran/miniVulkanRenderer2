#pragma once
#include"vk_common.h"


namespace mini
{
class Device;
class DeviceMemory;

class Buffer
{
public:
	Buffer(Device& device,uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	~Buffer();
	VkBuffer getHandle();
	uint32_t getSize();
	void map(const void* rawData, uint32_t size);

private:
	uint32_t size;
	void bindBufferMemory(const DeviceMemory& deviceMemory);
	VkBuffer handle{ VK_NULL_HANDLE };
	std::unique_ptr<DeviceMemory> deviceMemory;
	Device& device;
};
}