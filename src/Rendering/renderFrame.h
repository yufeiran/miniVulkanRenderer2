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
class DescriptorPool;
class DescriptorSet;
class DescriptorSetLayout;
class Buffer;
class ResourceManagement;

/*
* Render Frame is a container for per-frame data, including BufferPool objects, synchronization primitives (semaphores , fences) and the swapchain RenderTarget.
* 每个Frame都存一个自己的DescriptorPool 和 DescriptorSets
* 
*/
class RenderFrame
{
public:

	RenderFrame(Device& device, ResourceManagement& resourceManagement,std::unique_ptr<RenderTarget>&& renderTarget,const RenderPass& renderPass,
		std::vector<std::unique_ptr<DescriptorSetLayout>>& descriptorSetLayouts);

	Device& getDevice();

	FrameBuffer& getFrameBuffer();

	void reset();

	void createUniformBuffer();

	void setImageInfos();

	void updateUniformBuffer();



	void createDescriptorSets();

	const VkExtent2D getExtent() const;

	std::vector<std::unique_ptr<DescriptorSet>>& getDescriptorSets();

private:
	Device& device;

	ResourceManagement& resourceManagement;

	std::unique_ptr<RenderTarget> renderTarget;

	std::unique_ptr<FrameBuffer> frameBuffer;

	std::unique_ptr<DescriptorPool> descriptorPool;

	std::vector<std::unique_ptr<DescriptorSetLayout>>& descriptorSetLayouts;

	std::vector<std::unique_ptr<Buffer>>uniformBuffers;


	BindingMap<VkDescriptorBufferInfo> bufferInfos;

	BindingMap<VkDescriptorImageInfo> imageInfos;
};
}