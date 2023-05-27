#pragma once 
#include"vk_common.h"

namespace mini
{
class Device;
class RenderTarget;
class RenderPass;

class Framebuffer
{
public:
	Framebuffer(Device& device,RenderTarget&renderTarget,RenderPass&renderPass);

	~Framebuffer();

	VkFramebuffer getHandle() const;
private:
	VkFramebuffer handle{ VK_NULL_HANDLE };

	Device& device;

	VkExtent2D extent{};
};


}