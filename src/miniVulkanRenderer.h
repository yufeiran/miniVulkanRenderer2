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


using namespace mini;
class MiniVulkanRenderer
{
public:
	MiniVulkanRenderer();

	void init(int width = 1024, int height = 768);

	void loop();

	void createSwapChainImagesAndImageViews();

	~MiniVulkanRenderer();

private:
	int width, height;
	
	std::unique_ptr<Instance> instance;

	std::unique_ptr<GlfwWindow> window;

	std::unique_ptr<PhysicalDevice> physicalDevice;

	VkSurfaceKHR surface;

	std::unique_ptr<Device> device;

	std::unique_ptr<Swapchain> swapchain;

	std::vector<std::unique_ptr<Image>> swapChainImages;

	std::vector<std::unique_ptr<ImageView>> swapChainImageViews;

	std::vector<std::unique_ptr<ShaderModule>>shaderModules;

	std::unique_ptr<GraphicPipeline>graphicPipeline;

};