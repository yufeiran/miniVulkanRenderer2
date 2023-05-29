#include"miniVulkanRenderer.h"
#include<chrono>

using namespace mini;
using namespace std::chrono;

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

	shaderModules.push_back(std::make_unique<ShaderModule>("../../shaders/vertexShader.vert.spv", *device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../shaders/fragmentShader.frag.spv", *device, VK_SHADER_STAGE_FRAGMENT_BIT));

	graphicPipeline = std::make_unique<GraphicPipeline>(shaderModules, *device, renderContext->getSurfaceExtent(), renderContext->getFormat());
	
	renderContext->prepare(graphicPipeline->getRenderPass());

	Log("miniVulkanRenderer init finish");
}

void MiniVulkanRenderer::loop()
{

	while(!window->shouldClose()){

		window->processEvents();
		drawFrame();

		frameCount++;

		std::string title = "miniVulkanRenderer2 fps:";
		title += toString(calFps());
		window->setTitle(title.c_str());

	}
	device->waitIdle();
}

void MiniVulkanRenderer::drawFrame()
{
	auto result= renderContext->beginFrame();
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		handleSizeChange();
		return;
	}

	auto& cmd = renderContext->getCurrentCommandBuffer();
	auto& frame = renderContext->getActiveFrame();
	
	recordCommandBuffer(cmd, frame.getFrameBuffer());

	renderContext->submit(device->getGraphicQueue(), &cmd);

	renderContext->endFrame();
}

void MiniVulkanRenderer::recordCommandBuffer(CommandBuffer& cmd, FrameBuffer& frameBuffer)
{
	cmd.reset();
	cmd.begin();
	cmd.beginRenderPass(graphicPipeline->getRenderPass(), frameBuffer);
	cmd.bindPipeline(*graphicPipeline);
	cmd.setViewPortAndScissor(frameBuffer.getExtent());
	cmd.draw(3, 1, 0, 0);
	cmd.endRenderPass();
	cmd.end();


}

double MiniVulkanRenderer::calFps()
{
	static auto last = std::chrono::system_clock::now();
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> dur = now - last;
	double frameTime = double(dur.count())*0.001;
	double fps = 1.0 / frameTime;
	last = now;

	return fps;
}

void MiniVulkanRenderer::handleSizeChange()
{
	device->waitIdle();

	auto extent=window->getExtent();
	while (extent.width == 0 || extent.height == 0)
	{
		extent = window->getExtent();
		window->waitEvents();
	}
	width = extent.width;
	height = extent.height;

	renderContext.reset();
	renderContext = std::make_unique<RenderContext>(*device, surface, *window);

	graphicPipeline.reset();
	graphicPipeline = std::make_unique<GraphicPipeline>(shaderModules, *device, renderContext->getSurfaceExtent(), renderContext->getFormat());

	renderContext->prepare(graphicPipeline->getRenderPass());

}


MiniVulkanRenderer::~MiniVulkanRenderer()
{
	Log("Renderer shutting down");
}
