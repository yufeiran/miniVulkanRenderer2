#pragma once 

#include"vk_common.h"

namespace mini
{
class Device;

class Semaphore
{
public:
	Semaphore(Device& device, VkSemaphoreCreateFlags flag = {});
	~Semaphore();
	VkSemaphore getHandle() const;
private:
	VkSemaphore handle{ VK_NULL_HANDLE };
	Device& device;

};

}