

#include <glm/gtx/matrix_decompose.hpp>
#include "miniVulkanRenderer.h"
#include <chrono>
#include "Common/common.h"
#include "Vulkan/shaderInfo.h"
#include "ResourceManagement/ResourceManager.h"
#include "Vulkan/sampler.h"
#include <glm/gtx/euler_angles.hpp>

#include <stb_image.h>


#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"


using namespace mini;
using namespace std::chrono;

void MiniVulkanRenderer::load()
{

	int testCase = 2;
	scene.load(testCase, *rm, camera);

}

MiniVulkanRenderer::MiniVulkanRenderer()
{
	checkMaterialSize();
	initLogFile("mini.log");

	volkInitialize();
}


void MiniVulkanRenderer::init(int width, int height)
{

	srand(time(0));

	LogLogo();
	Log("init start");
	LogTimerStart("init");

	width = width;
	height = height;

	window = std::make_unique<GUIWindow>(width, height, "miniVulkanRenderer2");
	window->setApp(this);
	window->setMouseCallBack(mouseCallBack);
	window->setJoystickCallBack(joystickCallback);
	window->setMouseButtonCallBack(mouseButtonCallback);
	window->setMouseScrollCallBack(mouseScrollCallback);
	window->setDropCallback(dropCallback);

	LogSpace();
	instance = std::make_unique<Instance>();

	surface = window->createSurface(instance->getHandle());

	auto& gpu = instance->getFirstGpu();

	physicalDevice = std::make_shared<PhysicalDevice>(gpu);

	defaultSurfaceDepthFormat = gpu.findDepthFormat();

	std::vector<const char*> deviceExtension = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME ,
		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
		VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME,
		VK_KHR_SHADER_CLOCK_EXTENSION_NAME
	};

	device = std::make_unique<Device>(gpu, surface, deviceExtension);

	canRaytracing = device->enableRayTracing();


	LogSpace();

	renderContext = std::make_unique<RenderContext>(*device, surface, *window);

	tempCommandPool = std::make_unique<CommandPool>(*device);

	rm = std::make_unique<ResourceManager>(*device);


	std::vector<std::string> defaultCubeMapNames = {
	getAssetPath("skybox/default/right.jpg"),
	getAssetPath("skybox/default/left.jpg"),
	getAssetPath("skybox/default/top.jpg"),
	getAssetPath("skybox/default/bottom.jpg"),
	getAssetPath("skybox/default/front.jpg"),
	getAssetPath("skybox/default/back.jpg"),
	};

	std::vector<std::string> yokohamaCubeMapNames = {
		getAssetPath("skybox/Yokohama3/posx.jpg"),
		getAssetPath("skybox/Yokohama3/negx.jpg"),
		getAssetPath("skybox/Yokohama3/posy.jpg"),
		getAssetPath("skybox/Yokohama3/negy.jpg"),
		getAssetPath("skybox/Yokohama3/posz.jpg"),
		getAssetPath("skybox/Yokohama3/negz.jpg")
	};


	std::vector<std::string> HornstullsStrandCubeMapNames = {
	getAssetPath("skybox/HornstullsStrand/posx.jpg"),
	getAssetPath("skybox/HornstullsStrand/negx.jpg"),
	getAssetPath("skybox/HornstullsStrand/posy.jpg"),
	getAssetPath("skybox/HornstullsStrand/negy.jpg"),
	getAssetPath("skybox/HornstullsStrand/posz.jpg"),
	getAssetPath("skybox/HornstullsStrand/negz.jpg")
	};




	std::string dikhololo_night_4k_Names = getAssetPath("HDRI/dikhololo_night_4k.hdr");

	std::string dikhololo_night_8k_Names = getAssetPath("HDRI/dikhololo_night_8k.hdr");

	std::string qwantani_moon_noon_puresky_4k_Names = getAssetPath("HDRI/qwantani_moon_noon_puresky_4k.hdr");


	Log("Load scene");
	LogTimerStart("Load scene");


	load();
	rm->loadCubemap(HornstullsStrandCubeMapNames);
	rm->loadHDR(qwantani_moon_noon_puresky_4k_Names);

	LogTimerEnd("Load scene");

	LogSpace();


	makeCubeMapPipeline = std::make_unique<MakeCubeMapPipeline>(*device, *rm, pcRaster);

	makeCubeMapPipeline->updateDescriptorSet(rm->getHdrImageView());

	graphicsPipelineBuilder = std::make_unique<GraphicsPipelineBuilder>(*device, *rm, *renderContext, offscreenColorFormat, pcRaster);



	createOffScreenFrameBuffer();

	shadowPipelineBuilder = std::make_unique<ShadowPipelineBuilder>(*device, *rm, pcRaster, graphicsPipelineBuilder->getLightUniformsBuffer());

	ssaoPipelineBuilder = std::make_unique<SSAOPipelineBuilder>(*device, *rm, window->getFramebufferSize(), graphicsPipelineBuilder->getDescriptorSetLayout(),
		*offscreenRenderTarget, offscreenColorFormat, pcRaster);

	ssrPipelineBuilder = std::make_unique<SSRPipelineBuilder>(*device, *rm, window->getFramebufferSize(), graphicsPipelineBuilder->getDescriptorSetLayout(),
		*offscreenRenderTarget, offscreenColorFormat, pcRaster);

	pbbloomPipelineBuilder = std::make_unique<PBBloomPipelineBuilder>(*device,
		*rm,
		window->getFramebufferSize(),
		*offscreenRenderTarget,
		offscreenColorFormat,
		pcPost, 5);

	rayPipe = std::make_unique<RayTracingPipelineBuilder>();

	auto& shadowMapRenderTarget = shadowPipelineBuilder->getDirRenderTarget();
	auto& PointShadowMapRenderPass = shadowPipelineBuilder->getPointRenderTarget();
	auto& cubemapImageView = makeCubeMapPipeline->getCubeMapRenderTarget().getImageViewByIndex(0);
	auto& diffuseIrrImageView = makeCubeMapPipeline->getDiffuseIrradianceRenderTarget().getImageViewByIndex(0);

	graphicsPipelineBuilder->updateDescriptorSet(shadowMapRenderTarget, PointShadowMapRenderPass, *offscreenRenderTarget,
		cubemapImageView, diffuseIrrImageView);


	initPostRender(renderContext->getSwapchainImageFormat());


	updatePostDescriptorSet();


	std::vector<std::shared_ptr<DescriptorSetLayout>> layouts{ graphicsPipelineBuilder->getDescriptorSetLayout() };
	renderContext->prepare(*postRenderPass, *rm, layouts,
		graphicsPipelineBuilder->getForwardRenderPass().getGraphicsPipeline().getShaderModules().front()->getShaderInfo());

	ui.initImGui(*instance, *physicalDevice, *device, renderContext->getSwapchain(), *postRenderPass);
	
	ImGui_ImplGlfw_InitForVulkan(window->getHandle(), true);

	if (canRaytracing && enableRayTracing)
	{
		rayPipe->buildRayTracing(*device,*physicalDevice,*rm,*offscreenRenderTarget,*graphicsPipelineBuilder->getDescriptorSetLayout());
	}


	LogSpace();

	window->showWindow();


	addLight(lights, *rm, LIGHT_TYPE_DIRECTIONAL, { 0.f, 20.f, 2.f }, { 0.f, -1.f, 0.f }, { 1.f, 1.f, 1.f }, 2.5f, true);

	LogTimerEnd("init");
}

void MiniVulkanRenderer::loop()
{
	if (makeCubeMapEveryTime == false)
	{
		auto& cmd = renderContext->getCurrentCommandBuffer();
		cmd.reset();
		cmd.begin();

		// makeHDRToCubeMap
		makeCubeMapPipeline->draw(cmd);
		cmd.end();


		cmd.submitAndWaitIdle(device->getGraphicQueue());
	}

	std::vector<VkClearValue> clearValues(3);
	VkClearColorValue defaultClearColor = { 106.0f / 256,131.0f / 256,114.0f / 256,1.0f };
	clearValues[0].color = defaultClearColor;
	clearValues[1].depthStencil = { 1.0f,0 };
	clearValues[2].depthStencil = { 1.0f,0 };

	static bool sizeChange = true;
	static bool lightSizeChange = true;
	static bool changeToRaytracing = false;
	static bool lastCanRaytracingMode = useRaytracing;

	int lastHeight = window->getFramebufferSize().height;
	int lastWidth = window->getFramebufferSize().width;


	while (!window->shouldClose()) {
		calFps();
	
		if (useRaytracing != lastCanRaytracingMode)
		{
			lastCanRaytracingMode = useRaytracing;
			if (useRaytracing == true)
			{
				changeToRaytracing = true;
			}
		}



		if (lightSizeChange || changeToRaytracing)
		{
			device->waitIdle();
			LogTimerStart("rebuild RT_TLAS");
			if (useRaytracing)
			{
				rayPipe->createTopLevelAS(*rm);
				rayPipe->updateRtDescriptorSet(*device,*offscreenRenderTarget,*postRenderImageSampler);
				rayPipe->updateInstances(*rm);
			}


			LogTimerEnd("rebuild RT_TLAS");
			lightSizeChange = false;
			changeToRaytracing = false;
			continue;

		}

		//updateInstances();


		auto result = renderContext->beginFrame();
		int nowHeight = window->getFramebufferSize().height;
		int nowWidth = window->getFramebufferSize().width;
		if ((nowHeight != lastHeight || nowWidth != lastWidth) || (result == VK_ERROR_OUT_OF_DATE_KHR))
		{
			lastHeight = nowHeight;
			lastWidth = nowWidth;

			handleSizeChange();
			sizeChange = true;
			Log("Size change!");
			continue;


		}


		pcRaster.screenHeight = window->getFramebufferSize().height;
		pcRaster.screenWidth = window->getFramebufferSize().width;

		auto& cmd = renderContext->getCurrentCommandBuffer();
		auto& renderFrame = renderContext->getActiveFrame();

		const auto& swapChainFormat = renderContext->getFormat();

		auto& frameBuffer = renderFrame.getFrameBuffer();



		cmd.reset();
		cmd.begin();

		surfaceExtent = renderContext->getSwapchain().getExtent();

		if (makeCubeMapEveryTime)
		{
			makeCubeMapPipeline->draw(cmd);
		}

		graphicsPipelineBuilder->update(cmd, camera, surfaceExtent, lights);



		// Raster render pass
		{
			
			if (useRaytracing)
			{
				rayPipe->updateFrame(camera, pcRay, window->getFramebufferSize());
				if (pcRay.frame >= maxFrames)
					return;
				glm::vec4 clearColor = { clearValues[0].color.float32[0],
										clearValues[0].color.float32[1],
										clearValues[0].color.float32[2],
										clearValues[0].color.float32[3] };
				rayPipe->raytrace(cmd, clearColor, pcRay, graphicsPipelineBuilder->getDescriptorSet(), surfaceExtent);
				
			}
			else
			{
				rasterize(cmd, defaultClearColor);
			}

		}

		// Offscreen render pass
		{
			auto& gs = getGlobalMiniSetting();
			if (gs.bloom_on) {
				pbbloomPipelineBuilder->draw(cmd);
			}

			cmd.beginRenderPass(*postRenderPass, frameBuffer, clearValues);
			cmd.bindPipeline(*postPipeline);

			cmd.bindDescriptorSet({ postDescriptorSet });
			//cmd.pushConstant(pcPost,static_cast<VkShaderStageFlagBits>( ));

			//void CommandBuffer::pushConstant(PushConstantPost& pushConstant,VkShaderStageFlagBits stage)

			vkCmdPushConstants(cmd.getHandle(), postPipelineLayout->getHandle(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantPost), &pcPost);
			cmd.bindVertexBuffer(postQuad->getVertexBuffer());

			cmd.setViewPortAndScissor(frameBuffer.getExtent());

			cmd.draw(3, 1, 0, 0);
			cmd.draw(3, 1, 1, 0);

			// ImGui 
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			bool changed = false;

			changed = ui.renderUI(clearValues, window->getFramebufferSize(), sizeChange, lightSizeChange, useRaytracing,
				pcRay, pcRaster, pcPost, camera,lights,*rm);
			if (changed)
			{
				resetFrame();
			}

			ImGui::Render();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd.getHandle());

			cmd.endRenderPass();
		}

		cmd.end();
		renderContext->submit(device->getGraphicQueue(), &cmd);
		renderContext->endFrame();

		sizeChange = false;

		processIO();
	}
	device->waitIdle();
}

void MiniVulkanRenderer::updateSceneDescriptors()
{
	auto& cubeMapImageView = makeCubeMapPipeline->getCubeMapRenderTarget().getImageViewByIndex(0);
	auto& diffuseIrrImageView = makeCubeMapPipeline->getDiffuseIrradianceRenderTarget().getImageViewByIndex(0);
	graphicsPipelineBuilder->updateDescriptorSet(shadowPipelineBuilder->getDirRenderTarget(), shadowPipelineBuilder->getPointRenderTarget(), *offscreenRenderTarget
		, cubeMapImageView, diffuseIrrImageView);
}

void MiniVulkanRenderer::rasterize(CommandBuffer& cmd, VkClearColorValue defaultClearColor)
{
	auto& gs = getGlobalMiniSetting();

	std::vector<VkClearValue>clearValues = std::vector<VkClearValue>(11);

	clearValues[0].color = defaultClearColor;
	clearValues[1].depthStencil = { 1.0f,0 };
	clearValues[2].color = { 0,0,0 };
	clearValues[3].color = { 0,0,0 };
	clearValues[4].color = { 0,0,0 };
	clearValues[5].color = { 0,0,0 };
	clearValues[6].color = { 0,0,0 };
	clearValues[7].color = { 0,0,0 };
	clearValues[8].color = { 0,0,0 };
	clearValues[9].color = { 0,0,0 };
	clearValues[10].color = { 0,0,0 };

	shadowPipelineBuilder->draw(cmd);

	auto& rasterRenderPass = graphicsPipelineBuilder->getRasterRenderPass();
	cmd.beginRenderPass(rasterRenderPass, *offscreenFramebuffer, clearValues);
	graphicsPipelineBuilder->draw(cmd);
	cmd.endRenderPass();


	if (gs.ssr_on)
	{
		ssrPipelineBuilder->draw(cmd, graphicsPipelineBuilder->getDescriptorSet());
	}


}

void MiniVulkanRenderer::processIO()
{
	keyControl();
	joystickControl();
	window->processEvents();
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

	float moveCameraSpeed = 500;
	auto& camera = miniRenderer.getCamera();

	static float speedRate = 1.0;


	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.move(UP_DIR, deltaTime * speedRate);
	}
	if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.move(DOWN_DIR, deltaTime * speedRate);
	}
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.move(LEFT_DIR, deltaTime * speedRate);
	}
	if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.move(RIGHT_DIR, deltaTime * speedRate);
	}
	if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.move(FRONT_DIR, deltaTime * speedRate);
	}
	if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera.move(END_DIR, deltaTime * speedRate);
	}
	if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(win, GLFW_TRUE);
	}
	if (glfwGetKey(win, GLFW_KEY_I) == GLFW_PRESS)
	{
		camera.changeDir(0, moveCameraSpeed * deltaTime * speedRate);
	}
	if (glfwGetKey(win, GLFW_KEY_K) == GLFW_PRESS)
	{
		camera.changeDir(0, -moveCameraSpeed * deltaTime * speedRate);
	}
	if (glfwGetKey(win, GLFW_KEY_J) == GLFW_PRESS)
	{
		camera.changeDir(-moveCameraSpeed * deltaTime * speedRate, 0);
	}
	if (glfwGetKey(win, GLFW_KEY_L) == GLFW_PRESS)
	{
		camera.changeDir(moveCameraSpeed * deltaTime * speedRate, 0);
	}
	if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speedRate = 3.0;
	}
	if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speedRate = 1.0;
	}

}

bool isLeftMouseButtonPress = false;
bool isRightMouseButtonPress = false;
bool isMiddleMouseButtonPress = false;


void MiniVulkanRenderer::mouseControl()
{
	static bool firstMouse = true;


	static double lastX, lastY;

	double xpos, ypos;

	const auto& win = window->getHandle();



	glfwGetCursorPos(win, &xpos, &ypos);

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

	Log("xoffset:" + std::to_string(xoffset) + " yoffset:" + std::to_string(yoffset));

	if (isLeftMouseButtonPress == true)
	{


		auto& camera = miniRenderer.getCamera();


		camera.changeDir(xoffset, yoffset);
	}




	lastX = xpos;
	lastY = ypos;

}

void MiniVulkanRenderer::joystickControl()
{
	const auto& win = window->getHandle();
	auto& camera = miniRenderer.getCamera();
	static float lastFrame = glfwGetTime();

	float currentFrame = glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	//Log("Joystick/Gamepad 1 status" + toString(present));
	if (1 == present)
	{
		if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
		{
			double sensitivity = 3;
			GLFWgamepadstate state;
			if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
			{
				if (state.buttons[GLFW_GAMEPAD_BUTTON_A])
				{
					//Log("Press A");
					camera.move(FRONT_DIR, deltaTime * sensitivity);
				}
				if (state.buttons[GLFW_GAMEPAD_BUTTON_B])
				{
					//Log("Press B");
					camera.move(END_DIR, deltaTime * sensitivity);
				}
				if (state.buttons[GLFW_GAMEPAD_BUTTON_START])
				{
					glfwSetWindowShouldClose(win, GLFW_TRUE);
				}


			}
			double upSpeed;
			double sideSpeed;
			double cameraUpSpeed;
			double cameraSideSpeed;
			upSpeed = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
			sideSpeed = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
			cameraUpSpeed = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
			cameraSideSpeed = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
			double threshold = 0.2;

			if (abs(upSpeed) > threshold)
			{
				if (upSpeed > 0)
				{
					camera.move(DOWN_DIR, abs(upSpeed) * deltaTime * sensitivity);
				}
				else
				{
					camera.move(UP_DIR, abs(upSpeed) * deltaTime * sensitivity);
				}
			}
			if (abs(sideSpeed) > threshold)
			{
				if (sideSpeed > 0)
				{
					camera.move(RIGHT_DIR, abs(sideSpeed) * deltaTime * sensitivity);
				}
				else
				{
					camera.move(LEFT_DIR, abs(sideSpeed) * deltaTime * sensitivity);
				}
			}
			if (abs(cameraUpSpeed) > threshold || abs(cameraSideSpeed) > threshold)
			{
				camera.changeDir(cameraSideSpeed * 1000 * deltaTime, -cameraUpSpeed * 1000 * deltaTime);

			}
		}
	}
}


void MiniVulkanRenderer::mouseCallBack(GLFWwindow* window, double xpos, double ypos)
{
	auto app = static_cast<MiniVulkanRenderer*>(glfwGetWindowUserPointer(window));

	static bool firstMouse = true;


	static double lastX, lastY;


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

	//Log("xoffset:" + std::to_string(xoffset) + " yoffset:" + std::to_string(yoffset));

	const double dirSensitivity = 2.1;
	const double moveSensitivity = 0.003;


	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureMouse)
	{
		return;
	}


	if (isLeftMouseButtonPress == true)
	{


		auto& camera = miniRenderer.getCamera();


		camera.changeDir(xoffset * dirSensitivity, yoffset * dirSensitivity);
	}



	if (ImGui::GetIO().WantCaptureMouse == false) {


	}
	auto midButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	if (isMiddleMouseButtonPress == true)
	{

		auto& camera = miniRenderer.getCamera();
		camera.move(FRONT_DIR, yoffset * moveSensitivity);
		camera.move(RIGHT_DIR, xoffset * moveSensitivity);
	}
	auto rightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (isRightMouseButtonPress == true)
	{
		float offset = sqrt(xoffset * xoffset + yoffset * yoffset) * moveSensitivity;
		auto& camera = miniRenderer.getCamera();
		if (xoffset + yoffset > 0)
		{
			camera.move(UP_DIR, offset);
		}
		else
		{
			camera.move(DOWN_DIR, offset);
		}

	}
}

void MiniVulkanRenderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		isLeftMouseButtonPress = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		isLeftMouseButtonPress = false;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		isRightMouseButtonPress = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		isRightMouseButtonPress = false;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		isMiddleMouseButtonPress = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		isMiddleMouseButtonPress = false;
	}
}

void MiniVulkanRenderer::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureMouse)
	{
		return;
	}


	auto app = static_cast<MiniVulkanRenderer*>(glfwGetWindowUserPointer(window));
	auto& camera = app->getCamera();

	float moveSensitivity = 0.1;
	camera.move(UP_DIR, yoffset * moveSensitivity);
}

void MiniVulkanRenderer::joystickCallback(int jid, int event)
{


	if (event == GLFW_CONNECTED)
	{
		const char* name = glfwGetJoystickName(jid);
		Log("Joystick #" + toString(jid) + " name:" + name + " is CONNECTED!");
	}
	else if (event == GLFW_DISCONNECTED)
	{
		const char* name = glfwGetJoystickName(jid);
		Log("Joystick #" + toString(jid) + " name:" + name + " is DISCONNECTED!");
	}


}

void MiniVulkanRenderer::dropCallback(GLFWwindow* window, int count, const char** path)
{

	auto app = static_cast<MiniVulkanRenderer*>(glfwGetWindowUserPointer(window));

	vkDeviceWaitIdle(app->device->getHandle());
	app->resetFrame();
	app->rm->clearScene();
	for (int i = 0; i < app->lights.size(); i++) {
		int newInstanceId = app->rm->addLightCubeInstance(app->lights[i], i);
		app->lights[i].setInstanceId(newInstanceId);
	}

	const char* filename_ = path[0];
	std::string filename = filename_;
	auto offset = filename.find_last_of('.');
	std::string extName = filename.substr(offset);
	if (extName == ".gltf")
	{
		glm::mat4 objMat = glm::mat4(1.0f);
		objMat = glm::mat4(1.0f);
		app->rm->loadScene(filename,objMat,true);
	}
	if (app->canRaytracing == true && app->enableRayTracing == true)
	{
		app->rayPipe->buildAS(*app->rm);
	}
	app->graphicsPipelineBuilder->createObjDescriptionBuffer();

	app->graphicsPipelineBuilder->updateDescriptorSet(
		app->shadowPipelineBuilder->getDirRenderTarget(),
		app->shadowPipelineBuilder->getPointRenderTarget(),
		*app->offscreenRenderTarget,
		app->makeCubeMapPipeline->getCubeMapRenderTarget().getImageViewByIndex(0),
		app->makeCubeMapPipeline->getDiffuseIrradianceRenderTarget().getImageViewByIndex(0)
	);

	if (app->canRaytracing && app->enableRayTracing) {
		app->rayPipe->updateRtDescriptorSet(*app->device,*app->offscreenRenderTarget,*app->postRenderImageSampler);
	}
	app->device->waitIdle();
	Log("Reload Finish!");
}

void MiniVulkanRenderer::cleanScene()
{
	resetFrame();
	device->waitIdle();



	renderContext.reset();
	renderContext = std::make_unique<RenderContext>(*device, surface, *window);

	surfaceExtent = renderContext->getSurfaceExtent();

	rm.reset();
	rm = std::make_unique<ResourceManager>(*device);


	createOffScreenFrameBuffer();
	rayPipe->updateRtDescriptorSet(*device, *offscreenRenderTarget, *postRenderImageSampler);
	updatePostDescriptorSet();


	const auto& descSetLayout = graphicsPipelineBuilder->getDescriptorSetLayout();
	auto& rasterPipeline = graphicsPipelineBuilder->getForwardRenderPass().getGraphicsPipeline();
	std::vector<std::shared_ptr<DescriptorSetLayout>> layouts{ descSetLayout };
	renderContext->prepare(*postRenderPass, *rm, layouts
		, rasterPipeline.getShaderModules().front()->getShaderInfo());
}


void MiniVulkanRenderer::handleSizeChange()
{
	resetFrame();
	device->waitIdle();

	auto extent = window->getFramebufferSize();
	while (extent.width == 0 || extent.height == 0)
	{
		extent = window->getFramebufferSize();
		window->waitEvents();
	}
	width = extent.width;
	height = extent.height;
	frameCount = 0;

	// inform ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)width, (float)height);
	auto winExtent = window->getWindowSize();
	if (width > 0 && height > 0)
	{
		io.DisplayFramebufferScale = ImVec2((float)width / winExtent.width, (float)height / winExtent.height);
	}


	renderContext.reset();
	renderContext = std::make_unique<RenderContext>(*device, surface, *window);

	surfaceExtent = renderContext->getSurfaceExtent();


	createOffScreenFrameBuffer();

	if (canRaytracing == true && enableRayTracing == true)
	{
		rayPipe->updateRtDescriptorSet(*device,*offscreenRenderTarget,*postRenderImageSampler);
	}


	updatePostDescriptorSet();

	auto& cubeMapImageView = makeCubeMapPipeline->getCubeMapRenderTarget().getImageViewByIndex(0);
	auto& diffuseIrrImageView = makeCubeMapPipeline->getDiffuseIrradianceRenderTarget().getImageViewByIndex(0);

	graphicsPipelineBuilder->rebuild(extent);
	graphicsPipelineBuilder->updateDescriptorSet(shadowPipelineBuilder->getDirRenderTarget(), shadowPipelineBuilder->getPointRenderTarget(), *offscreenRenderTarget
		, cubeMapImageView, diffuseIrrImageView);

	ssrPipelineBuilder->rebuild(extent, *offscreenRenderTarget);

	pbbloomPipelineBuilder->rebuild(extent, *offscreenRenderTarget, 5);

	updatePostDescriptorSet();



	//rasterPipeline.reset();
 //   rasterPipeline = std::make_unique<GraphicPipeline>(rasterShaderModules,*rasterPipelineLayout,*device,surfaceExtent);
	//rasterPipeline->build(*rasterRenderPass);

	const auto& descSetLayout = graphicsPipelineBuilder->getDescriptorSetLayout();
	auto& rasterPipeline = graphicsPipelineBuilder->getForwardRenderPass().getGraphicsPipeline();
	std::vector<std::shared_ptr<DescriptorSetLayout>> layouts{ descSetLayout };
	renderContext->prepare(*postRenderPass, *rm, layouts
		, rasterPipeline.getShaderModules().front()->getShaderInfo());



}

Camera& MiniVulkanRenderer::getCamera()
{
	return camera;
}

void MiniVulkanRenderer::createOffScreenFrameBuffer()
{
	surfaceExtent = window->getFramebufferSize();

	std::vector<Image> images;

	auto imageColor = Image(*device, surfaceExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT);


	auto depthFormat = imageColor.getDevice().getPhysicalDevice().findDepthFormat();
	std::unique_ptr<Image> depthImage = std::make_unique<Image>(imageColor.getDevice(),
		imageColor.getExtent(), depthFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);


	// GBuffer
	auto imagePos = Image(*device, surfaceExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
	auto imageNormal = Image(*device, surfaceExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
	auto imageAlbedoSpec = Image(*device, surfaceExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
	auto imageMetalRough = Image(*device, surfaceExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
	auto imageEmissive = Image(*device, surfaceExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
	auto imageSSAO = Image(*device, surfaceExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
	auto imageSSAOBlur = Image(*device, surfaceExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
	auto imagePosViewSpace = Image(*device, surfaceExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
	auto imageNormalViewSpace = Image(*device, surfaceExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);

	images.push_back(std::move(imageColor));
	images.push_back(std::move(*depthImage));
	images.push_back(std::move(imagePos));
	images.push_back(std::move(imageNormal));
	images.push_back(std::move(imageAlbedoSpec));
	images.push_back(std::move(imageMetalRough));
	images.push_back(std::move(imageEmissive));
	images.push_back(std::move(imageSSAO));
	images.push_back(std::move(imageSSAOBlur));
	images.push_back(std::move(imagePosViewSpace));
	images.push_back(std::move(imageNormalViewSpace));

	offscreenRenderTarget = std::make_unique<RenderTarget>(std::move(images));
	auto& rasterRenderPass = graphicsPipelineBuilder->getRasterRenderPass();
	offscreenFramebuffer = std::make_unique<FrameBuffer>(*device, *offscreenRenderTarget, rasterRenderPass);

	//auto depthFormat = device->getPhysicalDevice().findDepthFormat();
 //  std::unique_ptr<Image> depthImage = std::make_unique<Image>(*device,
 //          	surfaceExtent, depthFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

   //shadowMapRenderTarget=RenderTarget::DEFAULT_CREATE_FUNC(std::move(*depthImage));
   //auto& rasterRenderPass = graphicsPipelineBuilder->getRasterRenderPass();
   //shadowMapFramebuffer=std::make_unique<FrameBuffer>(*device,*shadowMapRenderTarget,rasterRenderPass);

}


void MiniVulkanRenderer::initPostRender(VkFormat postSurfaceColorFormat)
{
	// create post pipeline!

	ShaderInfo postShaderInfo;
	postShaderInfo.bindingInfoMap[0][1] = BindingInfo{ TEXTURE_BINDING_TYPE,DIFFUSE };

	postShaderModules.push_back(std::make_unique<ShaderModule>(getSPVPath("post.vert.spv"), *device, VK_SHADER_STAGE_VERTEX_BIT));
	postShaderModules.push_back(std::make_unique<ShaderModule>(getSPVPath("post.frag.spv"), *device, VK_SHADER_STAGE_FRAGMENT_BIT));

	for (auto& s : postShaderModules)
	{
		s->setShaderInfo(postShaderInfo);
	}

	std::vector<VkPushConstantRange> pushConstants;

	VkPushConstantRange pushConstant = {};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantPost);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);

	postDescSetBind.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	postDescSetBind.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	postDescSetBind.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	postDescSetBind.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);  // debug SSR


	postDescriptorSetLayouts.push_back(std::move(postDescSetBind.createLayout(*device)));

	postPipelineLayout = std::make_unique<PipelineLayout>(*device, postDescriptorSetLayouts, pushConstants);

	std::vector<Attachment> attachments;
	{
		Attachment colorAttachment{ postSurfaceColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		attachments.push_back(colorAttachment);

		Attachment depthAttachment{ device->getPhysicalDevice().findDepthFormat(),VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT };
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachments.push_back(depthAttachment);


	}

	std::vector<LoadStoreInfo> loadStoreInfos;
	{
		LoadStoreInfo colorLoadStoreInfo{};
		colorLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(colorLoadStoreInfo);

		LoadStoreInfo depthLoadStoreInfo{};
		depthLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		loadStoreInfos.push_back(depthLoadStoreInfo);


	}

	postRenderPass = std::make_unique<RenderPass>(*device, attachments, loadStoreInfos);


	surfaceExtent = renderContext->getSurfaceExtent();
	auto swapChainFormat = renderContext->getFormat();
	postPipeline = std::make_unique<GraphicsPipeline>(postShaderModules, *postPipelineLayout, *device, surfaceExtent);

	postPipeline->rasterizer.cullMode = VK_CULL_MODE_NONE;

	postPipeline->build(*postRenderPass);

	// create quad
	postQuad = std::make_unique<PostQuad>(*device);

	// create descriptor set
	postDescriptorPool = std::make_unique<DescriptorPool>(*device);
	BindingMap<VkDescriptorImageInfo> imageInfos;
	BindingMap<VkDescriptorBufferInfo> bufferInfos;



	const auto& offscreenColorImageView = offscreenRenderTarget->getImageViewByIndex(0);
	const auto& offscreenColorImage = offscreenRenderTarget->getImageByIndex(0);

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	postRenderImageSampler = std::make_unique<Sampler>(*device);
	imageInfo.imageView = offscreenColorImageView.getHandle();


	imageInfo.sampler = postRenderImageSampler->getHandle();
	imageInfos[0][1] = imageInfo;

	postDescriptorSet = postDescriptorPool->allocate(*postDescriptorSetLayouts[0], bufferInfos, imageInfos).getHandle();

	VkDescriptorSetLayoutBinding postSamplerLayoutBinding{};
	postSamplerLayoutBinding.binding = 1;
	postSamplerLayoutBinding.descriptorCount = 1;
	postSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	postSamplerLayoutBinding.pImmutableSamplers = nullptr;
	postSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding postSamplerShadowMapLayoutBinding{};
	postSamplerLayoutBinding.binding = 2;
	postSamplerLayoutBinding.descriptorCount = 1;
	postSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	postSamplerLayoutBinding.pImmutableSamplers = nullptr;
	postSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding postSamplerBloomLayoutBinding{};
	postSamplerBloomLayoutBinding.binding = 3;
	postSamplerBloomLayoutBinding.descriptorCount = 1;
	postSamplerBloomLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	postSamplerBloomLayoutBinding.pImmutableSamplers = nullptr;
	postSamplerBloomLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding postSamplerSSRLayoutBinding{};
	postSamplerSSRLayoutBinding.binding = 4;
	postSamplerSSRLayoutBinding.descriptorCount = 1;
	postSamplerSSRLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	postSamplerSSRLayoutBinding.pImmutableSamplers = nullptr;
	postSamplerSSRLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


	std::vector<VkDescriptorSetLayoutBinding>postLayoutBindings{ postSamplerLayoutBinding,postSamplerShadowMapLayoutBinding,postSamplerBloomLayoutBinding,postSamplerSSRLayoutBinding };

	postDescriptorSetLayouts.push_back(std::make_unique<DescriptorSetLayout>(*device, postLayoutBindings));

}

void MiniVulkanRenderer::updatePostDescriptorSet()
{
	const auto& offscreenColorImageView = offscreenRenderTarget->getImageViewByIndex(0);

	std::vector<VkWriteDescriptorSet> writes;


	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imageInfo.imageView = offscreenColorImageView.getHandle();
	imageInfo.sampler = postRenderImageSampler->getHandle();
	VkWriteDescriptorSet writeDescriptorSets = postDescSetBind.makeWrite(postDescriptorSet, 1, &imageInfo);

	writes.push_back(writeDescriptorSets);

	auto& renderTarget = shadowPipelineBuilder->getDirRenderTarget();

	VkDescriptorImageInfo shadowMapInfo;
	shadowMapInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	shadowMapInfo.imageView = renderTarget.getImageViewByIndex(0).getHandle();
	shadowMapInfo.sampler = rm->getDefaultSampler().getHandle();

	writes.push_back(postDescSetBind.makeWrite(postDescriptorSet, 2, &shadowMapInfo));



	VkDescriptorImageInfo imageBloomInfo{};
	imageBloomInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imageBloomInfo.imageView = pbbloomPipelineBuilder->getRenderTargets()[0]->getImageViewByIndex(0).getHandle();
	imageBloomInfo.sampler = postRenderImageSampler->getHandle();

	writes.push_back(postDescSetBind.makeWrite(postDescriptorSet, 3, &imageBloomInfo));

	VkDescriptorImageInfo imageSSRInfo{};
	imageSSRInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imageSSRInfo.imageView = ssrPipelineBuilder->getRenderTarget().getImageViewByIndex(1).getHandle();
	imageSSRInfo.sampler = postRenderImageSampler->getHandle();

	writes.push_back(postDescSetBind.makeWrite(postDescriptorSet, 4, &imageSSRInfo));



	vkUpdateDescriptorSets(device->getHandle(), writes.size(), writes.data(), 0, nullptr);

}

void MiniVulkanRenderer::resetFrame()
{
	pcRay.frame = -1;
}

void MiniVulkanRenderer::updateFrame()
{
	static glm::mat4 refCamMat;
	static float  refFov = camera.getFov();

	const auto& m = camera.getViewMat();
	const auto fov = camera.getFov();

	if (memcmp(&refCamMat[0][0], &m[0][0], sizeof(glm::mat4)) != 0 || refFov != fov)
	{
		resetFrame();
		refCamMat = m;
		refFov = fov;
	}
	pcRay.frame++;
	pcRay.totalFrameCount = frameCount;
	
	float fovY_radians = glm::radians(camera.getFov());
	float pixelSpreadAngle = 2.0f * tan(fovY_radians / 2.0f) / float(window->getFramebufferSize().height);
	pcRay.pixelSpreadAngle = pixelSpreadAngle;
}



MiniVulkanRenderer::~MiniVulkanRenderer()
{
	Log("Renderer shutting down");

	rayPipe.reset();
	graphicsPipelineBuilder.reset();


	if (ImGui::GetCurrentContext() != nullptr)
	{
		ImGui_ImplVulkan_Shutdown();
	}
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	rm.reset();
	offscreenRenderTarget.reset();

	closeLogFile();
}




void MiniVulkanRenderer::calFps()
{
	static double avgFps = 0;

	static double lastTime = 0;
	static double lastFrameCount = 0;
	static auto last = std::chrono::system_clock::now();
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> dur = now - last;
	double frameTime = double(dur.count()) * 0.001;
	double fps = 1.0 / frameTime;
	last = now;

	frameCount++;
	lastFrameCount++;

	VkExtent2D extent = window->getFramebufferSize();

	std::string title = "miniVulkanRenderer2 " + std::to_string(extent.width) + "x" + std::to_string(extent.height) + " avg fps:";


	lastTime += frameTime;

	if (lastTime >= 1.0)
	{
		avgFps = lastFrameCount;
		lastFrameCount = 0;
		lastTime = 0;
	}


	title += toString(avgFps);
	title += " fps:";
	title += toString(fps);
	window->setTitle(title.c_str());

}