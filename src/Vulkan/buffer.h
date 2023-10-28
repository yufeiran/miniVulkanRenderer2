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

	VkBuffer getHandle();

	uint32_t getSize();

	// 从cpu内存中拷贝数据到buffer的GPU内存中，先map 再copy 最后 unmap
	void map(const void* rawData, uint32_t size);

	// 持久的map 到mapAddress上
	void persistentMap(uint32_t size);

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