#include "buffer.h"
#include"device.h"
#include"deviceMemory.h"

namespace mini
{



Buffer::Buffer(Device& device,  uint32_t size, VkBufferUsageFlags usage,VkMemoryPropertyFlags properties)
	:device(device),size(0)
{
	VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device.getHandle(), &bufferInfo, nullptr, &handle) != VK_SUCCESS) 
	{
		throw Error("Failed to create buffer!");
	}

	deviceMemory = std::make_unique<DeviceMemory>(device,*this, properties);

	bindBufferMemory(*deviceMemory);


}

Buffer::~Buffer()
{
	if (handle != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(device.getHandle(), handle, nullptr);
	}
}

VkBuffer Buffer::getHandle()
{
	return handle;
}

uint32_t Buffer::getSize()
{
	return size;
}

void Buffer::map(const void* rawData, uint32_t size)
{
	void* data;
	vkMapMemory(device.getHandle(), deviceMemory->getHandle(), 0, size, 0, &data);
	memcpy(data, rawData, (size_t)size);
	vkUnmapMemory(device.getHandle(), deviceMemory->getHandle());
	this->size = size;
}

void Buffer::bindBufferMemory(const DeviceMemory& deviceMemory)
{
	vkBindBufferMemory(device.getHandle(), handle, deviceMemory.getHandle(), 0);
}

}


