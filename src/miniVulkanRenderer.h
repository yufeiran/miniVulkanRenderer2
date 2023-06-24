#pragma once
#include<iostream>
#include"Common/common.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_ZERO_TO_ONE
#include"glm/vec4.hpp"
#include"glm/mat4x4.hpp"
#include"Common/miniLog.h"
#include"Common/camera.h"
#include"Platform/glfwWindow.h"
#include"Vulkan/instance.h"
#include"Vulkan/physicalDevice.h"
#include"Vulkan/deviceMemory.h"
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
#include"ResourceManagement/resourceManagement.h"
#include"Vulkan/descriptorSetLayout.h"
#include"Vulkan/descriptorPool.h"
#include"Vulkan/descriptorSet.h"
#include"Vulkan/buffer.h"
#include"Sprite/sprite.h"
#include"Sprite/spriteList.h"




//这是miniVulkanRenderer2
/* 一个基础的
*/

using namespace mini;
class MiniVulkanRenderer
{
public:
	MiniVulkanRenderer();

	~MiniVulkanRenderer();

	void init(int width = 1024, int height = 768);

	void loop();

	void keyControl();

	void joystickControl();

	void drawFrame();

	void recordCommandBuffer(CommandBuffer& cmd, RenderFrame& renderFrame);

	static void mouseCallBack(GLFWwindow* window, double xpos, double ypos);

	static void joystickCallback(int jid, int event);

	double calFps();

	void handleSizeChange();

	Camera& getCamera();


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

	std::vector<std::unique_ptr<DescriptorSetLayout>> descriptorSetLayouts;

	std::unique_ptr<GraphicPipeline>graphicPipeline;

	std::unique_ptr<ResourceManagement> resourceManagement;

	SpriteList spriteList;

	Camera  camera;


};

extern MiniVulkanRenderer miniRenderer;