#include"semaphore.h"
#include"device.h"

namespace mini
{
Semaphore::Semaphore(Device& device, VkSemaphoreCreateFlags flag):device(device)
{
	VkSemaphoreCreateInfo semaphoreInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	semaphoreInfo.flags = flag;
	if (vkCreateSemaphore(device.getHandle(), &semaphoreInfo, nullptr, &handle) != VK_SUCCESS) {
		throw Error("Failed to create semaphores!");
	}
}

Semaphore::~Semaphore()
{
	if (handle != VK_NULL_HANDLE)
	{
		vkDestroySemaphore(device.getHandle(), handle, nullptr);
	}
}

VkSemaphore Semaphore::getHandle() const
{
	return handle;
}

}