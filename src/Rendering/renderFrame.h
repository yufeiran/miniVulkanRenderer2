#pragma once
#include"Common/common.h"
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
class DescriptorPool;
class DescriptorSet;
class DescriptorSetLayout;
class Buffer;
class ResourceManager;
class ShaderInfo;
class Model;
class Shape;
class Camera;

/*
* Render Frame is a container for per-frame data, including BufferPool objects, synchronization primitives (semaphores , fences) and the swapchain RenderTarget.
* ÿ��Frame����һ���Լ���DescriptorPool �� DescriptorSets
* 
*/
class RenderFrame
{
public:

	RenderFrame(Device& device, ResourceManager& resourceManager,std::unique_ptr<RenderTarget>&& renderTarget,const RenderPass& renderPass,
		std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts, ShaderInfo &shaderInfo);

	Device& getDevice();

	FrameBuffer& getFrameBuffer();

	void reset();


	const VkExtent2D getExtent() const;

private:
	Device& device;

	ResourceManager& resourceManager;

	std::unique_ptr<RenderTarget> renderTarget;

	std::unique_ptr<FrameBuffer> frameBuffer;

};
}