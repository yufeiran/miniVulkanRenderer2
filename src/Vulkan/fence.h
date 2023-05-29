#pragma once

#include"vk_common.h"

namespace mini
{
class Device;

class Fence
{
public:
	Fence(Device& device, VkFenceCreateFlags flag ={});
	~Fence();

	void wait();

	void reset();

	VkFence getHandle() const;
private:
	Device& device;
	VkFence handle{ VK_NULL_HANDLE };
};

}