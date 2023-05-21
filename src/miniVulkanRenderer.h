#pragma once
#include<iostream>
#include"Vulkan/vk_common.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_ZERO_TO_ONE
#include"glm/vec4.hpp"
#include"glm/mat4x4.hpp"
#include"Common/miniLog.h"
#include"Vulkan/instance.h"

using namespace mini;
class MiniVulkanRenderer
{
public:
	MiniVulkanRenderer();
	void init(int width = 1024, int height = 768);
	void loop();
	~MiniVulkanRenderer();
private:
	int mWidth, mHeight;

	GLFWwindow* window = nullptr;
	std::unique_ptr<Instance> mInstance;
};