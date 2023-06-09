#pragma once
#include"Vulkan/vk_common.h"
#include"renderTarget.h"
#include"renderFrame.h"

namespace mini
{
class Device;
class GlfwWindow;
class RenderFrame;
class Queue;
class Swapchain;
class RenderTarget;
class Image;
class Semaphore;
class CommandPool;
class CommandBuffer;
class DescriptorPool;
class DescriptorSetLayout;

/* RenderContext acts as a frame manager 
*  �����ӹ�swapchain�����𴴽�renderFrame���ҹ���renderFrame����������
*  
*/
class RenderContext
{
public:

	static VkFormat DEFAULT_VK_FORMAT;

	RenderContext(Device& device, VkSurfaceKHR surface, const GlfwWindow& window);

	void prepare(const RenderPass& renderPass,
		ResourceManagement& resourceManagement,
		std::vector<std::unique_ptr<DescriptorSetLayout>>& descriptorSetLayouts,
		RenderTarget::CreateFunc createRenderTargetFunc= RenderTarget::DEFAULT_CREATE_FUNC
		);

	VkExtent2D const& getSurfaceExtent() const;

	VkFormat getFormat() const;

	RenderFrame& getActiveFrame();


	VkResult beginFrame();

	void submit(const Queue& queue, const CommandBuffer* cmd);

	void endFrame();

	void waitFrame();

	CommandBuffer& getCurrentCommandBuffer();

	uint32_t getCurrentFrames();

private:
	Device& device;

	const GlfwWindow& window;

	const Queue& queue;

	std::unique_ptr<Swapchain> swapchain;

	std::vector<std::unique_ptr<RenderFrame>> frames;

	std::unique_ptr<RenderTarget> depthRenderTarget;

	// Current active frame index 
	uint32_t activeFrameIndex{ 0 };

	VkExtent2D surfaceExtent;

	bool frameActive{ false };

	std::unique_ptr<CommandPool> commandPool;

	std::vector<std::unique_ptr<CommandBuffer>>commandBuffers;

	std::vector<std::unique_ptr<Semaphore>> imageAvailableSemaphores; //�������ƣ���swapchainȡ��ͼƬ֮����ܽ��л��ƵĹ���
	std::vector<std::unique_ptr<Semaphore>> renderFinishedSemaphores; //�������ƣ���Draw��ɺ��ͼƬ���ܱ�չʾ����

	std::vector<std::unique_ptr<Fence>> inFlightFences; //�������ƣ���GPU���Draw��������CPU���µ���Ⱦ�����GPU


	uint32_t currentFrames = 0;

	
};

}