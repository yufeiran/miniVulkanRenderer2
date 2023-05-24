#pragma once
#include"Vulkan/vk_common.h"

namespace mini
{

class Instance;

class GlfwWindow
{
public:
	GlfwWindow(int width,int height,const char* title);

	~GlfwWindow();

	GLFWwindow* getHandle() const;

	VkSurfaceKHR createSurface(VkInstance instance);

	VkExtent2D getExtent() const;

	bool shouldClose() const;

	void processEvent() const;



private:
	VkInstance instance{ VK_NULL_HANDLE };

	GLFWwindow* handle = nullptr;

	VkSurfaceKHR surface{ VK_NULL_HANDLE };


	int width;

	int height;

};


}