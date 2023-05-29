#pragma once
#include"Vulkan/vk_common.h"
#include"Vulkan/framebuffer.h"

namespace mini
{
class Device;
class RenderTarget;
class RenderPass;
class Semaphore;
class Fence;
class CommandPool;
class CommandBuffer;

/*
* Render Frame is a container for per-frame data, including BufferPool objects, synchronization primitives (semaphores , fences) and the swapchain RenderTarget.
* 
* 
*/
class RenderFrame
{
public:

	RenderFrame(Device& device,std::unique_ptr<RenderTarget>&& renderTarget,const RenderPass& renderPass);

	Device& getDevice();

	FrameBuffer& getFrameBuffer();

	void reset();



private:
	Device& device;

	std::unique_ptr<RenderTarget> renderTarget;
	std::unique_ptr<FrameBuffer> frameBuffer;




};
}