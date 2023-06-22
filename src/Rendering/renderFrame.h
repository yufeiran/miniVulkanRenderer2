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
class ResourceManagement;
class ShaderInfo;
class Model;
class Shape;
class Camera;

/*
* Render Frame is a container for per-frame data, including BufferPool objects, synchronization primitives (semaphores , fences) and the swapchain RenderTarget.
* 每个Frame都存一个自己的DescriptorPool 和 DescriptorSets
* 
*/
class RenderFrame
{
public:

	RenderFrame(Device& device, ResourceManagement& resourceManagement,std::unique_ptr<RenderTarget>&& renderTarget,const RenderPass& renderPass,
		std::vector<std::unique_ptr<DescriptorSetLayout>>& descriptorSetLayouts, ShaderInfo &shaderInfo);

	Device& getDevice();

	FrameBuffer& getFrameBuffer();

	void reset();

	void createUniformBuffer();



	void updateUniformBuffer(Camera &c);



	void createDescriptorSets(ResourceManagement&resourceManagement,ShaderInfo& shaderInfo);

	const VkExtent2D getExtent() const;

	std::vector<VkDescriptorSet>& getDescriptorSet(Model& m,Shape& s);

private:
	Device& device;

	ResourceManagement& resourceManagement;

	std::unique_ptr<RenderTarget> renderTarget;

	std::unique_ptr<FrameBuffer> frameBuffer;

	std::unique_ptr<DescriptorPool> descriptorPool;

	std::vector<std::unique_ptr<DescriptorSetLayout>>& descriptorSetLayouts;

	std::vector<std::unique_ptr<Buffer>>uniformBuffers;

	std::map < std::string, std::map<std::string,std::vector<VkDescriptorSet>>> descriptorSetMap;


	BindingMap<VkDescriptorBufferInfo> bufferInfos;

	BindingMap<VkDescriptorImageInfo> imageInfos;
};
}