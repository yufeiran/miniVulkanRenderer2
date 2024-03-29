#pragma once
#include"Common/common.h"
#include"renderTarget.h"
#include"renderFrame.h"

namespace mini
{
class Device;
class GUIWindow;
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
class ShaderInfo;
/* RenderContext acts as a frame manager 
*  用来接管swapchain，负责创建renderFrame并且管理renderFrame的生命周期
*  
*/
class RenderContext
{
public:

	static VkFormat DEFAULT_VK_FORMAT;

	RenderContext(Device& device, VkSurfaceKHR surface, const GUIWindow& window);

	void prepare(const RenderPass& renderPass,
		ResourceManager& resourceManager,
		std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts,
		ShaderInfo& shaderInfo,
		RenderTarget::CreateFunc createRenderTargetFunc= RenderTarget::DEFAULT_CREATE_FUNC
		);

	VkExtent2D const& getSurfaceExtent() const;

	VkFormat getFormat() const;

	RenderFrame& getActiveFrame();

	Swapchain& getSwapchain() const{return *swapchain;}


	VkResult beginFrame();

	void submit(const Queue& queue, const CommandBuffer* cmd);

	void endFrame();

	void waitFrame();

	CommandBuffer& getCurrentCommandBuffer();

	uint32_t getCurrentFrames();

private:
	Device& device;

	const GUIWindow& window;

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

	std::vector<std::unique_ptr<Semaphore>> imageAvailableSemaphores; //用来控制：从swapchain取到图片之后才能进行绘制的过程
	std::vector<std::unique_ptr<Semaphore>> renderFinishedSemaphores; //用来控制：当Draw完成后的图片才能被展示出来

	std::vector<std::unique_ptr<Fence>> inFlightFences; //用来控制：当GPU完成Draw工作后让CPU发新的渲染任务给GPU


	uint32_t currentFrames = 0;

	
};

}