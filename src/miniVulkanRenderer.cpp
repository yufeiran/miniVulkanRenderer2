#include"miniVulkanRenderer.h"

using namespace mini;



MiniVulkanRenderer::MiniVulkanRenderer()
{
	volkInitialize();
}



void MiniVulkanRenderer::init(int width, int height)
{
	width = width;
	height = height;
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(width, height, "miniVulkanRenderer2", nullptr, nullptr);



	glm::mat4 matrix;
	glm::vec4 vec;
	auto test = matrix * vec;

	instance = std::make_unique<Instance>();

	auto& gpu = instance->getFirstGpu();


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
