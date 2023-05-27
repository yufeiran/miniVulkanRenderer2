#pragma once
#include"Vulkan/vk_common.h"

namespace mini
{
class Device;
class RenderTarget;

/*
* Render Frame is a container for per-frame data, including BufferPool objects, synchronization primitives (semaphores , fences) and the swapchain RenderTarget.
* 
* 
*/
class RenderFrame
{
public:

	RenderFrame(Device& device,std::unique_ptr<RenderTarget>&& renderTarget);

	Device& getDevice();

private:
	Device& device;

	std::unique_ptr<RenderTarget> swapchainRenderTarget;


};
}