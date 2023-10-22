#pragma once
#include"Common/common.h"

namespace mini
{

class Instance;

enum CURSOR_MODE {
	DISABLED_CURSOR,
	HIDDEN_CURSOR,
	NORMAL_CURSOR
};

class GUIWindow
{
public:
	GUIWindow(int width,int height,const char* title);

	~GUIWindow();

	void joystickInput();

	GLFWwindow* getHandle() const;

	VkSurfaceKHR createSurface(VkInstance instance);

	VkExtent2D getExtent() const;

	void setTitle(const char* name);

	void setMouseCallBack(GLFWcursorposfun callback);

	void setMouseButtonCallBack(GLFWmousebuttonfun callback);

	void setJoystickCallBack(GLFWjoystickfun callback);

	void setCursorMode(CURSOR_MODE mode);

	CURSOR_MODE getCursorMode();

	bool shouldClose() const;

	void processEvents() const;

	void waitEvents() const;

	void loadIcon(const char* filename);

	void hideWindow() const;

	void showWindow() const;


private:
	VkInstance instance{ VK_NULL_HANDLE };

	GLFWwindow* handle = nullptr;

	VkSurfaceKHR surface{ VK_NULL_HANDLE };

	GLFWimage iconImage{};

	CURSOR_MODE nowCursorMode = NORMAL_CURSOR;


	int width;

	int height;

};


}