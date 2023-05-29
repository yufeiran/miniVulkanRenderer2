#pragma once 
#include"vk_common.h"

namespace mini
{
class Device;
class RenderTarget;
class RenderPass;

class FrameBuffer
{
public:
	FrameBuffer(Device& device,RenderTarget&renderTarget,const RenderPass&renderPass);

	~FrameBuffer();

	VkExtent2D getExtent() const;

	VkFramebuffer getHandle() const;
private:
	VkFramebuffer handle{ VK_NULL_HANDLE };

	Device& device;

	VkExtent2D extent{};
};


}