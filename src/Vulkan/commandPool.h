#pragma once
#include"vk_common.h"

namespace mini
{
class Device;


class CommandPool
{
public:

private:
	Device& device;
	VkCommandPool handle{ VK_NULL_HANDLE };
};

}