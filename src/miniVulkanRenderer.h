#pragma once
#include<iostream>
#include"Vulkan/vk_common.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_ZERO_TO_ONE
#include"glm/vec4.hpp"
#include"glm/mat4x4.hpp"
#include"Common/miniLog.h"
#include"Platform/glfwWindow.h"
#include"Vulkan/instance.h"
#include"Vulkan/physicalDevice.h"
#include"Vulkan/device.h"
#include"Vulkan/imageView.h"
#include"Vulkan/image.h"
#include"Vulkan/swapchain.h"
#include"Vulkan/shaderModule.h"
#include"Vulkan/graphicPipeline.h"
#include"Rendering/renderContext.h"
#include"Vulkan/commandPool.h"
#include"Vulkan/commandBuffer.h"
#include"Vulkan/framebuffer.h"
#include"Vulkan/fence.h"
#include"Vulkan/semaphore.h"


using namespace mini;
class MiniVulkanRenderer
{
public:
	MiniVulkanRenderer();

	~MiniVulkanRenderer();

	void init(int width = 1024, int height = 768);

	void loop();

	void drawFrame();

	void recordCommandBuffer(CommandBuffer& cmd, FrameBuffer& frameBuffer);

	double calFps();

	void handleSizeChange();



private:
	int width, height;

	long long frameCount=0;
	
	std::unique_ptr<Instance> instance;

	std::unique_ptr<GlfwWindow> window;

	std::unique_ptr<PhysicalDevice> physicalDevice;

	VkSurfaceKHR surface{};

	std::unique_ptr<Device> device;

	std::unique_ptr<RenderContext> renderContext;

	std::vector<std::unique_ptr<ShaderModule>>shaderModules;

	std::unique_ptr<GraphicPipeline>graphicPipeline;

};