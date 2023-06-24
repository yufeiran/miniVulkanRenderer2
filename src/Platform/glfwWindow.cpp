#include "glfwWindow.h"
#include"Vulkan/instance.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include"GLFW/glfw3native.h"

#include<fstream>


namespace mini
{

GlfwWindow::GlfwWindow(int width, int height, const char* title)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	handle = glfwCreateWindow(width, height, "miniVulkanRenderer2", nullptr, nullptr);
	loadIcon("../../assets/logo.png");
	
	setCursorMode(DISABLED_CURSOR);

	joystickInput();
}



GlfwWindow::~GlfwWindow()
{
	if (surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(instance, surface, nullptr);
	}
	glfwDestroyWindow(handle);

	if (iconImage.pixels != nullptr) 
	{
		stbi_image_free(iconImage.pixels);
	}

	glfwTerminate();
}

void GlfwWindow::joystickInput()
{
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);

	const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);

	int count;
	const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);


	
	const char* mappings;
	
	std::ifstream infile("../../assets/gamecontrollerdb.txt", std::ios::out | std::ios::in);

	std::ostringstream tmp;
	tmp << infile.rdbuf();
	std::string str = tmp.str();

	glfwUpdateGamepadMappings(str.c_str());

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

void GlfwWindow::setTitle(const char* name)
{
	glfwSetWindowTitle(handle, name);
}

void GlfwWindow::setMouseCallBack(GLFWcursorposfun callback)
{
	glfwSetCursorPosCallback(handle, callback);
}

void GlfwWindow::setJoystickCallBack(GLFWjoystickfun callback)
{
	glfwSetJoystickCallback(callback);
}

void GlfwWindow::setCursorMode(CURSOR_MODE mode)
{
	int value;
	if (mode == DISABLED_CURSOR) {
		value = GLFW_CURSOR_DISABLED;
	}
	else if (mode == HIDDEN_CURSOR) {
		value = GLFW_CURSOR_HIDDEN;
	}
	else if (mode == NORMAL_CURSOR) {
		value = GLFW_CURSOR_NORMAL;
	}
	glfwSetInputMode(handle, GLFW_CURSOR, value);
	nowCursorMode = mode;

}

CURSOR_MODE GlfwWindow::getCursorMode()
{
	return nowCursorMode;
}



bool GlfwWindow::shouldClose() const
{
	return glfwWindowShouldClose(handle);

}

void GlfwWindow::processEvents() const
{
	glfwPollEvents();
}

void GlfwWindow::waitEvents() const
{
	glfwWaitEvents();
}

void GlfwWindow::loadIcon(const char* filename)
{
	
	iconImage.pixels = stbi_load(filename, &iconImage.width, &iconImage.height, 0,4);
	glfwSetWindowIcon(handle, 1, &iconImage);
}

}
