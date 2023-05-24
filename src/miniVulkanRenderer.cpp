#include"miniVulkanRenderer.h"

using namespace mini;



MiniVulkanRenderer::MiniVulkanRenderer()
{
	volkInitialize();
}



void MiniVulkanRenderer::init(int width, int height)
{
	Log("miniVulkanRenderer init start");
	width = width;
	height = height;
	window = std::make_unique<GlfwWindow>(width, height, "miniVulkanRenderer2");



	glm::mat4 matrix;
	glm::vec4 vec;
	auto test = matrix * vec;

	instance = std::make_unique<Instance>();

	surface = window->createSurface(instance->getHandle());

	auto& gpu = instance->getFirstGpu();

	std::vector<const char*> deviceExtension = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	device = std::make_unique<Device>(gpu, surface, deviceExtension);

	swapchain = std::make_unique<Swapchain>(*device, surface, window->getExtent());

	createSwapChainImagesAndImageViews();

	Log("miniVulkanRenderer init finish");
}

void MiniVulkanRenderer::loop()
{
	while(!window->shouldClose()){
		window->processEvent();
	}
}

void MiniVulkanRenderer::createSwapChainImagesAndImageViews()
{
	auto swapchainImagesRaw = swapchain->getImages();
	auto swapchainExtent = swapchain->getExtent();
	auto swapchianFormat = swapchain->getImageFormat();

	for (auto image : swapchainImagesRaw)
	{
		swapChainImages.push_back(std::make_unique<Image>(image, *device, swapchainExtent, swapchianFormat));
	}

	for (auto& image : swapChainImages)
	{
		swapChainImageViews.push_back(std::make_unique<ImageView>(*image, swapchianFormat));
	}
}

MiniVulkanRenderer::~MiniVulkanRenderer()
{
	Log("Renderer shutting down");
}
