#include"miniVulkanRenderer.h"

using namespace mini;



MiniVulkanRenderer::MiniVulkanRenderer()
{
	volkInitialize();
}



void MiniVulkanRenderer::init(int width, int height)
{
	mWidth = width;
	mHeight = height;
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(mWidth, mHeight, "miniVulkanRenderer2", nullptr, nullptr);



	glm::mat4 matrix;
	glm::vec4 vec;
	auto test = matrix * vec;

	mInstance = std::make_unique<Instance>();
}

void MiniVulkanRenderer::loop()
{
	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
	}
}

MiniVulkanRenderer::~MiniVulkanRenderer()
{
	glfwDestroyWindow(window);

	glfwTerminate();

	Log("Renderer shutting down", WARNING);
}
