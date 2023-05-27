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





	instance = std::make_unique<Instance>();

	surface = window->createSurface(instance->getHandle());

	auto& gpu = instance->getFirstGpu();

	std::vector<const char*> deviceExtension = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	device = std::make_unique<Device>(gpu, surface, deviceExtension);

	renderContext = std::make_unique<RenderContext>(*device, surface, *window);

	renderContext->prepare();



	shaderModules.push_back(std::make_unique<ShaderModule>("../../shaders/vertexShader.vert.spv", *device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../shaders/fragmentShader.frag.spv", *device, VK_SHADER_STAGE_FRAGMENT_BIT));

	graphicPipeline = std::make_unique<GraphicPipeline>(shaderModules, *device, renderContext->getSurfaceExtent(), renderContext->getFormat());
	
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
	auto &swapchainImagesRaw = swapchain->getImages();
	auto swapchainExtent = swapchain->getExtent();
	auto swapchianFormat = swapchain->getImageFormat();
	auto swapchianUsage = swapchain->getImageUsage();

	for (auto image : swapchainImagesRaw)
	{
		swapChainImages.push_back(std::make_unique<Image>(*device,image,  swapchainExtent, swapchianFormat, swapchianUsage));
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
