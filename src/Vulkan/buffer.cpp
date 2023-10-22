#include "buffer.h"
#include"device.h"
#include"deviceMemory.h"

namespace mini
{



Buffer::Buffer(Device& device,  uint32_t size, VkBufferUsageFlags usage,VkMemoryPropertyFlags properties)
	:device(device),size(size),mapType(UNMAP)
{
	VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device.getHandle(), &bufferInfo, nullptr, &handle) != VK_SUCCESS) 
	{
		throw Error("Failed to create buffer!");
	}
	VkMemoryAllocateFlags flags={};
	if(hasFlag(usage,VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT))
	{
		flags|=VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
	}

	deviceMemory = std::make_unique<DeviceMemory>(device,*this, properties,flags);

	bindBufferMemory(*deviceMemory);


}

Buffer::~Buffer()
{
	//Log("~Buffer() call!");
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
	mapType = TEMP_MAPPING;
}



void Buffer::persistentMap(uint32_t size)
{
	vkMapMemory(device.getHandle(), deviceMemory->getHandle(), 0, size, 0, &mapAddress);
	mapType = PERSISTENT_MAPPING;
}

void* Buffer::getMapAddress()
{
	return mapAddress;
}

BufferMapType Buffer::getMapType()
{
	return mapType;
}

VkDeviceAddress Buffer::getBufferDeviceAddress()
{
	assert(handle!=VK_NULL_HANDLE);

	VkBufferDeviceAddressInfo info = {VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
	info.buffer = handle;

	return vkGetBufferDeviceAddress(device.getHandle(),&info);
}

void Buffer::bindBufferMemory(const DeviceMemory& deviceMemory)
{
	vkBindBufferMemory(device.getHandle(), handle, deviceMemory.getHandle(), 0);
}



}


