#include "deviceMemory.h"
#include "buffer.h"
#include "image.h"
#include "device.h"

namespace mini
{
uint32_t findMemoryType(Device& device, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;

	vkGetPhysicalDeviceMemoryProperties(device.getPhysicalDevice().getHandle(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{

		//找符合类型的内存，并且该内存还需要满足一些特性，比如可以用CPU访问..
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw Error("Failed to find sutiable memory type!");
}

DeviceMemory::DeviceMemory(Device& device,Buffer& buffer, VkMemoryPropertyFlags properties,VkMemoryAllocateFlags allocateFlags):
	device(device)
{
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device.getHandle(), buffer.getHandle(), &memRequirements);

	VkMemoryAllocateFlagsInfo flagInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO};
	flagInfo.flags = allocateFlags;


	VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(device, memRequirements.memoryTypeBits, properties);


	allocInfo.pNext=&flagInfo;

	if (vkAllocateMemory(device.getHandle(), &allocInfo, nullptr, &handle) != VK_SUCCESS)
	{
		throw Error("Failed to allocate device memory");
	}
}

DeviceMemory::DeviceMemory(Device& device, Image& image, VkMemoryPropertyFlags properties) :
	device(device)
{
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device.getHandle(), image.getHandle(), &memRequirements);

	VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(device, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device.getHandle(), &allocInfo, nullptr, &handle) != VK_SUCCESS)
	{
		throw Error("Failed to allocate device memory");
	}
}

DeviceMemory::~DeviceMemory()
{
	if (handle != VK_NULL_HANDLE)
	{
		vkFreeMemory(device.getHandle(), handle, nullptr);
	}
}

VkDeviceMemory DeviceMemory::getHandle() const
{
	return handle;
}

}

