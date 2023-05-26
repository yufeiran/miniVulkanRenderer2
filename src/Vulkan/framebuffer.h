#pragma once 
#include"vk_common.h"

namespace mini
{
class Device;

class Framebuffer
{
public:
	Framebuffer();

	~Framebuffer();

	VkFramebuffer getHandle() const;
private:
	VkFramebuffer handle{ VK_NULL_HANDLE };

	Device& device;

	VkExtent2D extent{};
};



}