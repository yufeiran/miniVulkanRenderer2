#pragma once
#include"Common/common.h"
#include"deviceMemory.h"

namespace mini
{
class Device;
class DeviceMemory;

enum BufferMapType{UNMAP,PERSISTENT_MAPPING,TEMP_MAPPING};
class Buffer
{
public:
	Buffer(Device& device,uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	~Buffer();

	VkBuffer getHandle();

	uint32_t getSize();

	// ��cpu�ڴ��п������ݵ�buffer��GPU�ڴ��У���map ��copy ��� unmap
	void map(const void* rawData, uint32_t size);

	// �־õ�map ��mapAddress��
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

	void* mapAddress; //ӳ�䵽�̶��ĵ�ַ

	BufferMapType mapType;

};
}