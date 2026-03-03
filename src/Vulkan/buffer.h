#pragma once
#include"Common/common.h"
#include"Vulkan/commandBuffer.h"
#include"deviceMemory.h"

namespace mini
{
class Device;
class DeviceMemory;

enum BufferMapType{UNMAP,PERSISTENT_MAPPING,TEMP_MAPPING};
class Buffer
{
public:
	Buffer(Device& device,VkDeviceSize size,VkBufferUsageFlags usage, VkMemoryPropertyFlags properties= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	Buffer(Device& device,VkDeviceSize size,const void * data,VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	template<class T>
	Buffer(Device& device,const std::vector<T> vec,VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT):Buffer(device,sizeof(T)* vec.size(),(const void*)vec.data(),usage,properties)
	{
		
	}

	~Buffer();

	VkDescriptorBufferInfo getDescriptorBufferInfo(VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);

	VkBuffer getHandle();

	uint32_t getSize();

	void setName(const char* name);

	void map(const void* rawData, uint32_t size);

	void persistentMap(uint32_t size);

	void unpersistentMap();

	void* getMapAddress();

	BufferMapType getMapType();

	VkDeviceAddress getBufferDeviceAddress();

private:



	uint32_t size;

	void bindBufferMemory(const DeviceMemory& deviceMemory);

	VkBuffer handle{ VK_NULL_HANDLE };

	std::unique_ptr<DeviceMemory> deviceMemory;

	Device& device;

	void* mapAddress; //映射到固定的地址

	BufferMapType mapType;

};
}