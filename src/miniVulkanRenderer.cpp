#include"miniVulkanRenderer.h"
#include<chrono>
#include"Vulkan/shaderInfo.h"
#include"ResourceManagement/texture.h"


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
	window->setMouseCallBack(mouseCallBack);

	instance = std::make_unique<Instance>();

	surface = window->createSurface(instance->getHandle());

	auto& gpu = instance->getFirstGpu();

	std::vector<const char*> deviceExtension = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	device = std::make_unique<Device>(gpu, surface, deviceExtension);

	renderContext = std::make_unique<RenderContext>(*device, surface, *window);

	ShaderInfo shaderInfo;
	shaderInfo.bindingInfoMap[0][1] = BindingInfo{ TEXTURE_BINDING_TYPE,DIFFUSE };

	shaderModules.push_back(std::make_unique<ShaderModule>("../../shaders/vertexShader.vert.spv", *device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../shaders/fragmentShader.frag.spv", *device, VK_SHADER_STAGE_FRAGMENT_BIT));
	
	for (auto& s : shaderModules) {
		s->setShaderInfo(shaderInfo);
	}
	
	
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::vector< VkDescriptorSetLayoutBinding>layoutBindings{ uboLayoutBinding ,samplerLayoutBinding };

	descriptorSetLayouts.push_back(std::make_unique<DescriptorSetLayout>(*device, layoutBindings));

	graphicPipeline = std::make_unique<GraphicPipeline>(shaderModules,descriptorSetLayouts, *device, renderContext->getSurfaceExtent(), renderContext->getFormat());
	
	resourceManagement = std::make_unique<ResourceManagement>(*device);

	resourceManagement->loadModel("BattleCruiser", "../../assets/BattleCruiser/BattleCruiser.obj");

	renderContext->prepare(graphicPipeline->getRenderPass(),*resourceManagement,descriptorSetLayouts,
		graphicPipeline->getShaderModules().front()->getShaderInfo());

	for (int i = 0; i < 10; i++)
	{
		spriteList.addSprite(resourceManagement->getModelByName("BattleCruiser"), -1, -1, -5 * i + 25, 1, 0, 90, 0);

	}


	Log("miniVulkanRenderer init finish");
}

void MiniVulkanRenderer::loop()
{
	double lastFps = 0;
	double avgFps = 0;

	while(!window->shouldClose()){
		keyControl();
		window->processEvents();
		drawFrame();

		frameCount++;

		std::string title = "miniVulkanRenderer2 avg fps:";
		double fps = calFps();
		if (frameCount % 1000 == 0)
		{
			avgFps = lastFps * (frameCount - 1) / (frameCount)+fps / frameCount;
		}

		lastFps = fps;
		title += toString(avgFps);
		title += " fps:";
		title += toString(fps);
		window->setTitle(title.c_str());

	}
	device->waitIdle();
}



void MiniVulkanRenderer::keyControl()
{
	const auto& win = window->getHandle();
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();

	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	static float lastFrame = glfwGetTime();

	float currentFrame = glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (glfwGetKey(win,GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.move(UP_DIR, deltaTime);
	}
	if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.move(DOWN_DIR, deltaTime);
	}
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.move(LEFT_DIR, deltaTime);
	}
	if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.move(RIGHT_DIR, deltaTime);
	}
	if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.move(FRONT_DIR, deltaTime);
	}
	if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera.move(END_DIR, deltaTime);
	}
	if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(win, GLFW_TRUE);
	}

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

	frame.updateUniformBuffer(camera);
	
	recordCommandBuffer(cmd, frame);

	renderContext->submit(device->getGraphicQueue(), &cmd);

	renderContext->endFrame();
}

void MiniVulkanRenderer::recordCommandBuffer(CommandBuffer& cmd, RenderFrame& renderFrame)
{
	auto& frameBuffer = renderFrame.getFrameBuffer();

	cmd.reset();
	cmd.begin();

	std::vector<VkClearValue> clearValues(2);
	clearValues[0].color = {0.0f,0.0f,0.0f,1.0f};
	clearValues[1].depthStencil = { 1.0f,0 };

	cmd.beginRenderPass(graphicPipeline->getRenderPass(), frameBuffer,clearValues);
	cmd.bindPipeline(*graphicPipeline);

	cmd.setViewPortAndScissor(frameBuffer.getExtent());

	for (auto& s : spriteList.sprites)
	{
		cmd.drawSprite(s, renderFrame);
	}

	cmd.endRenderPass();
	cmd.end();

}

void MiniVulkanRenderer::mouseCallBack(GLFWwindow* window, double xpos, double ypos)
{
	static bool firstMouse = true;


	static double lastX, lastY;

	glfwGetCursorPos(window, &xpos, &ypos);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	auto& camera = miniRenderer.getCamera();


	camera.changeDir(xoffset, yoffset);

	//if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	//{
	//	camera.changeDir(0, 1);
	//}
	//if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	//{
	//	camera.changeDir(0, -1);
	//}
	//if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	//{
	//	camera.changeDir(-1, 0);
	//}
	//if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	//{
	//	camera.changeDir(1, 0);
	//}

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
	frameCount = 0;

	renderContext.reset();
	renderContext = std::make_unique<RenderContext>(*device, surface, *window);

	graphicPipeline.reset();
	graphicPipeline = std::make_unique<GraphicPipeline>(shaderModules,descriptorSetLayouts, *device, renderContext->getSurfaceExtent(), renderContext->getFormat());

	renderContext->prepare(graphicPipeline->getRenderPass(),*resourceManagement,descriptorSetLayouts
	, graphicPipeline->getShaderModules().front()->getShaderInfo());

}

Camera& MiniVulkanRenderer::getCamera()
{
	return camera;
}


MiniVulkanRenderer::~MiniVulkanRenderer()
{
	Log("Renderer shutting down");
}
