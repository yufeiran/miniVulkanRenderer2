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

	void setTitle(const char* name);

	bool shouldClose() const;

	void processEvents() const;

	void waitEvents() const;

	void loadIcon(const char* filename);


private:
	VkInstance instance{ VK_NULL_HANDLE };

	GLFWwindow* handle = nullptr;

	VkSurfaceKHR surface{ VK_NULL_HANDLE };

	GLFWimage iconImage{};


	int width;

	int height;

};


}