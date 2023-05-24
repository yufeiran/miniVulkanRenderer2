#include "glfwWindow.h"
#include"Vulkan/instance.h"


#define GLFW_EXPOSE_NATIVE_WIN32
#include"GLFW/glfw3native.h"

namespace mini
{

GlfwWindow::GlfwWindow(int width, int height, const char* title)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	handle = glfwCreateWindow(width, height, "miniVulkanRenderer2", nullptr, nullptr);
}

GlfwWindow::~GlfwWindow()
{
	if (surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(instance, surface, nullptr);
	}
	glfwDestroyWindow(handle);



	glfwTerminate();
}

GLFWwindow* GlfwWindow::getHandle() const
{
	return handle;
}

VkSurfaceKHR GlfwWindow::createSurface(VkInstance instance)
{
	if (instance == VK_NULL_HANDLE || !handle)
	{
		return VK_NULL_HANDLE;
	}
	this->instance = instance;

	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = glfwGetWin32Window(handle);
	createInfo.hinstance = GetModuleHandle(nullptr);

	VkSurfaceKHR surface;

	if (glfwCreateWindowSurface(instance, handle, nullptr, &surface) != VK_SUCCESS) {
		throw Error("failed to create window surface!");
	}

	this->surface = surface;

	return surface;
}

VkExtent2D GlfwWindow::getExtent() const
{
	int width, height;
	glfwGetFramebufferSize(handle, &width, &height);
	return VkExtent2D{ static_cast<uint32_t>( width),static_cast<uint32_t>( height) };
}

bool GlfwWindow::shouldClose() const
{
	return glfwWindowShouldClose(handle);

}

void GlfwWindow::processEvent() const
{
	glfwPollEvents();
}

}
