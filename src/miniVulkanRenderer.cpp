#include"miniVulkanRenderer.h"
#include<chrono>
#include"Vulkan/shaderInfo.h"
#include"ResourceManagement/texture.h"
#include "Vulkan/sampler.h"



#include"imgui.h"
#include"imgui_impl_vulkan.h"
#include"imgui_impl_glfw.h"
#include "ResourceManagement/model.h"


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
	window = std::make_unique<GUIWindow>(width, height, "miniVulkanRenderer2");
	window->setMouseCallBack(mouseCallBack);
	window->setJoystickCallBack(joystickCallback);
	window->setMouseButtonCallBack(mouseButtonCallbcak);

	instance = std::make_unique<Instance>();

	surface = window->createSurface(instance->getHandle());

	auto& gpu = instance->getFirstGpu();

	physicalDevice=std::make_unique<PhysicalDevice>(*instance,gpu.getHandle());

	defaultDepthFormat =gpu.findDepthFormat();

	std::vector<const char*> deviceExtension = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME ,
		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
		VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME
	};

	device = std::make_unique<Device>(gpu, surface, deviceExtension);

	renderContext = std::make_unique<RenderContext>(*device, surface, *window);

	tempCommandPool = std::make_unique<CommandPool>(*device);

	initRasterRender();

	createOffScreenFrameBuffer();

	initPostRender();


	

	VkDescriptorSetLayoutBinding postSamplerLayoutBinding{};
	postSamplerLayoutBinding.binding = 1;
	postSamplerLayoutBinding.descriptorCount = 1;
	postSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	postSamplerLayoutBinding.pImmutableSamplers = nullptr;
	postSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::vector<VkDescriptorSetLayoutBinding>postLayoutBindings{postSamplerLayoutBinding};

	postDescriptorSetLayouts.push_back(std::make_unique<DescriptorSetLayout>(*device,postLayoutBindings));

	//postPipeline = std::make_unique<GraphicPipeline>(postShaderModules,postDescriptorSetLayouts,*device,
	//	renderContext->getSurfaceExtent(),renderContext->getFormat());




	resourceManagement = std::make_unique<ResourceManagement>(*device);

	//resourceManagement->loadModel("BattleCruiser", "../../assets/BattleCruiser/BattleCruiser.obj");
	resourceManagement->loadModel("backpack", "../../assets/backpack/backpack.obj",true);
	//resourceManagement->loadModel("Medieval_building", "../../assets/nv_raytracing_tutorial_scene/Medieval_building.obj",true);

	//resourceManagement->loadModel("plane", "../../assets/nv_raytracing_tutorial_scene/plane.obj",true);

	renderContext->prepare(*rasterRenderPass,*resourceManagement,rasterDescriptorSetLayouts,
		rasterPipeline->getShaderModules().front()->getShaderInfo());

	//for (int i = 0; i < 10; i++)
	//{
	//	spriteList.addSprite(resourceManagement->getModelByName("backpack"), -1, -1, -5 * i + 25, 1, 0, 90, 0);

	//}
	spriteList.addSprite(resourceManagement->getModelByName("backpack"));

	//spriteList.addSprite(resourceManagement->getModelByName("Medieval_building"));
	//spriteList.addSprite(resourceManagement->getModelByName("plane"));

	initImGUI();
	ImGui_ImplGlfw_InitForVulkan(window->getHandle(),true);

	initRayTracing();
	createBottomLevelAS();


	Log("miniVulkanRenderer init finish");
}

void MiniVulkanRenderer::initImGUI()
{

	ImGui::CreateContext();
	ImGuiIO& io   = ImGui::GetIO();
	io.IniFilename = nullptr; // Avoiding the INI file
	io.LogFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

	imguiDescPool = std::make_unique<DescriptorPool>(*device);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance			= instance->getHandle();
	init_info.PhysicalDevice	= physicalDevice->getHandle();
	init_info.Device			= device->getHandle();
	init_info.QueueFamily		= device->getGraphicQueue().getFamilyIndex();
	init_info.Queue				= device->getGraphicQueue().getHandle();
	init_info.PipelineCache		= VK_NULL_HANDLE;
	init_info.DescriptorPool    = imguiDescPool->getHandle();
	init_info.Subpass			= 0;
	init_info.MinImageCount		= 2;
	init_info.ImageCount		= static_cast<int>(renderContext->getSwapchain().getImageCount());
	init_info.MSAASamples		= VK_SAMPLE_COUNT_1_BIT;
	init_info.CheckVkResultFn	= nullptr;
	init_info.Allocator			= nullptr;

	ImGui_ImplVulkan_LoadFunctions(
		[](const char *function_name, void *vulkan_instance) {
						return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance *>(vulkan_instance)), function_name);}, &init_info.Instance);

	ImGui_ImplVulkan_Init(&init_info,postRenderPass->getHandle());

	auto& tempCmd = tempCommandPool->createCommandBuffer();
	tempCmd->beginSingleTime();
	ImGui_ImplVulkan_CreateFontsTexture(tempCmd->getHandle());
	tempCmd->endSingleTime(device->getGraphicQueue());

}

void MiniVulkanRenderer::initRayTracing()
{
	// Requesting ray tracing properties
	VkPhysicalDeviceProperties2 prop2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
	prop2.pNext = &rtProperties;
	vkGetPhysicalDeviceProperties2(physicalDevice->getHandle(),&prop2);
	rayTracingBuilder=std::make_unique<RayTracingBuilder>(*device,device->getGraphicQueue().getIndex());
}

auto MiniVulkanRenderer::shapeToVkGeometryKHR(const Shape& shape)
{
	RayTracingBuilder::BlasInput input;

	VkDeviceAddress vertexAddresss=shape.getVertexBuffer().getBufferDeviceAddress();
	VkDeviceAddress indexAddress =shape.getIndexBuffer().getBufferDeviceAddress();

	uint32_t maxPrimitiveCount = shape.indexSum/3;

	VkAccelerationStructureGeometryTrianglesDataKHR triangles{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR};
	triangles.vertexFormat			     = VK_FORMAT_R32G32B32_SFLOAT;
	triangles.vertexData.deviceAddress   = vertexAddresss;
	triangles.vertexStride				 = sizeof(Vertex);

	triangles.indexType                  = VK_INDEX_TYPE_UINT32;
	triangles.indexData.deviceAddress    = indexAddress;

	triangles.maxVertex = shape.vertexSum - 1;

	VkAccelerationStructureGeometryKHR asGeom{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR};
	asGeom.geometryType       = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	asGeom.flags              = VK_GEOMETRY_OPAQUE_BIT_KHR;
	asGeom.geometry.triangles = triangles;

	VkAccelerationStructureBuildRangeInfoKHR offset;

	offset.firstVertex = 0;
	offset.primitiveCount = maxPrimitiveCount;
	offset.primitiveOffset = 0;
	offset.transformOffset = 0;


	input.asGeometry.emplace_back(asGeom);
	input.asBuildOffsetInfo.emplace_back(offset);
	
	return input;

}

void MiniVulkanRenderer::createBottomLevelAS()
{
	std::vector<RayTracingBuilder::BlasInput> allBlas;
	//allBlas.reserve(resourceManagement->getModelSum());

	const auto& models = resourceManagement->getModelMap();
	for(const auto& modelPair:models)
	{
		const auto& model = modelPair.second;
		for(const auto& shapePair:model->getShapeMap())
		{
			const auto& shape = shapePair.second;
			auto blas = shapeToVkGeometryKHR(*shape);

			allBlas.push_back(blas);
		}

	}
	rayTracingBuilder->buildBlas(allBlas,VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
}

void MiniVulkanRenderer::loop()
{

	while(!window->shouldClose()){
		calFps();
		keyControl();
		joystickControl();
		window->processEvents();

		drawFrame();

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

	frame.updateUniformBuffer(camera);
	
	recordCommandBuffer(cmd, frame);

	renderContext->submit(device->getGraphicQueue(), &cmd);

	renderContext->endFrame();
}

void MiniVulkanRenderer::recordCommandBuffer(CommandBuffer& cmd, RenderFrame& renderFrame)
{
	auto& frameBuffer = renderFrame.getFrameBuffer();

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Hello World");
		ImGui::Text("miniVulkanRenderer2");
		ImGui::End();
	}
	

	cmd.reset();
	cmd.begin();

	std::vector<VkClearValue> clearValues(2);
	clearValues[0].color = {0.0f,0.0f,0.0f,1.0f};
	clearValues[1].depthStencil = { 1.0f,0 };



	// Raster render pass
	{
		cmd.beginRenderPass(*rasterRenderPass, *offscreenFramebuffer,clearValues);
		cmd.bindPipeline(*rasterPipeline);

		cmd.setViewPortAndScissor(frameBuffer.getExtent());



		for (auto& s : spriteList.sprites)
		{
			cmd.drawSprite(s, renderFrame);
		}

		cmd.endRenderPass();
	}




	// Offscreen render pass
	{
		cmd.beginRenderPass(*postRenderPass, frameBuffer,clearValues);
		cmd.bindPipeline(*postPipeline);

		cmd.bindDescriptorSet({postDescriptorSet});
		cmd.bindVertexBuffer(postQuad->getVertexBuffer());

		cmd.setViewPortAndScissor(frameBuffer.getExtent());

		cmd.draw(3,1,0,0);
		cmd.draw(3,1,1,0);

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),cmd.getHandle());

		cmd.endRenderPass();
	}

	cmd.end();
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

bool isLeftMouseButtonPress = false;


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
					camera.move(FRONT_DIR, deltaTime* sensitivity);
				}
				if (state.buttons[GLFW_GAMEPAD_BUTTON_B])
				{
					//Log("Press B");
					camera.move(END_DIR, deltaTime* sensitivity);
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
			double threshold=0.2;

			if (abs(upSpeed) > threshold)
			{
				if (upSpeed > 0)
				{
					camera.move(DOWN_DIR, abs(upSpeed) * deltaTime* sensitivity);
				}
				else
				{
					camera.move(UP_DIR, abs(upSpeed) * deltaTime* sensitivity);
				}
			}
			if (abs(sideSpeed) > threshold)
			{
				if (sideSpeed > 0)
				{
					camera.move(RIGHT_DIR, abs(sideSpeed) * deltaTime* sensitivity);
				}
				else
				{
					camera.move(LEFT_DIR, abs(sideSpeed) * deltaTime* sensitivity);
				}
			}
			if (abs(cameraUpSpeed) > threshold|| abs(cameraSideSpeed) > threshold)
			{
				camera.changeDir(cameraSideSpeed*1000* deltaTime, -cameraUpSpeed*1000* deltaTime);
			
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

	const double moveSensitivity = 2.1;
	xoffset *= moveSensitivity;
	yoffset *= moveSensitivity;

	ImGuiIO& io = ImGui::GetIO();

	if(io.WantCaptureMouse)
	{
		return;
	}


	if(isLeftMouseButtonPress==true)
	{


		auto& camera = miniRenderer.getCamera();


		camera.changeDir(xoffset, yoffset);
	}

	

}

void MiniVulkanRenderer::mouseButtonCallbcak(GLFWwindow* window, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		isLeftMouseButtonPress = true;
	}
	else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		isLeftMouseButtonPress = false;
	}
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

	surfaceExtent=renderContext->getSurfaceExtent();


	rasterPipeline.reset();
    rasterPipeline = std::make_unique<GraphicPipeline>(rasterShaderModules,*rasterPipelineLayout,*device,surfaceExtent);
	rasterPipeline->build(*rasterRenderPass);

	renderContext->prepare(*rasterRenderPass,*resourceManagement,rasterDescriptorSetLayouts
	, rasterPipeline->getShaderModules().front()->getShaderInfo());

}

Camera& MiniVulkanRenderer::getCamera()
{
	return camera;
}

void MiniVulkanRenderer::createOffScreenFrameBuffer()
{
	auto imageColor = Image(*device,surfaceExtent,defaultColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	offscreenRenderTarget=RenderTarget::DEFAULT_CREATE_FUNC(std::move(imageColor));
	offscreenFramebuffer=std::make_unique<FrameBuffer>(*device,*offscreenRenderTarget,*rasterRenderPass);

}

void MiniVulkanRenderer::initRasterRender()
{
	// create raster pipeline !

	ShaderInfo rasterShaderInfo;
	rasterShaderInfo.bindingInfoMap[0][1] = BindingInfo{ TEXTURE_BINDING_TYPE,DIFFUSE };

	rasterShaderModules.push_back(std::make_unique<ShaderModule>("../../shaders/vertexShader.vert.spv", *device, VK_SHADER_STAGE_VERTEX_BIT));
	rasterShaderModules.push_back(std::make_unique<ShaderModule>("../../shaders/fragmentShader.frag.spv", *device, VK_SHADER_STAGE_FRAGMENT_BIT));
	
	for (auto& s : rasterShaderModules) {
		s->setShaderInfo(rasterShaderInfo);
	}


	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant;
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantsMesh);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstants.push_back(pushConstant);

	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount=1;
	samplerLayoutBinding.descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	std::vector<VkDescriptorSetLayoutBinding>layoutBindings{uboLayoutBinding,samplerLayoutBinding};

	rasterDescriptorSetLayouts.push_back(std::make_unique<DescriptorSetLayout>(*device,layoutBindings));

	rasterPipelineLayout = std::make_unique<PipelineLayout>(*device,rasterDescriptorSetLayouts,pushConstants);

	rasterRenderPass = std::make_unique<RenderPass>(*device,defaultColorFormat,  VK_IMAGE_LAYOUT_GENERAL);


	surfaceExtent=renderContext->getSurfaceExtent();
	rasterPipeline = std::make_unique<GraphicPipeline>(rasterShaderModules,*rasterPipelineLayout,*device,surfaceExtent);

	rasterPipeline->build(*rasterRenderPass);

}

void MiniVulkanRenderer::initPostRender()
{
	// create post pipeline!

	ShaderInfo postShaderInfo;
	postShaderInfo.bindingInfoMap[0][1]=BindingInfo{TEXTURE_BINDING_TYPE,DIFFUSE};

	postShaderModules.push_back(std::make_unique<ShaderModule>("../../shaders/post.vert.spv",*device,VK_SHADER_STAGE_VERTEX_BIT));
	postShaderModules.push_back(std::make_unique<ShaderModule>("../../shaders/post.frag.spv",*device,VK_SHADER_STAGE_FRAGMENT_BIT));

	for(auto&s:postShaderModules)
	{
		s->setShaderInfo(postShaderInfo);
	}

	std::vector<VkPushConstantRange> pushConstants;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount=1;
	samplerLayoutBinding.descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	std::vector<VkDescriptorSetLayoutBinding>layoutBindings{samplerLayoutBinding};

	postDescriptorSetLayouts.push_back(std::make_unique<DescriptorSetLayout>(*device,layoutBindings));

	postPipelineLayout = std::make_unique<PipelineLayout>(*device,postDescriptorSetLayouts,pushConstants);

	postRenderPass = std::make_unique<RenderPass>(*device,defaultColorFormat);

	
	surfaceExtent=renderContext->getSurfaceExtent();
	postPipeline = std::make_unique<GraphicPipeline>(postShaderModules,*postPipelineLayout,*device,surfaceExtent);

	postPipeline->build(*postRenderPass);

	// create quad
	postQuad = std::make_unique<PostQuad>(*device);

	// create descriptor set
	postDescriptorPool = std::make_unique<DescriptorPool>(*device);
	BindingMap<VkDescriptorImageInfo> imageInfos;
	BindingMap<VkDescriptorBufferInfo> bufferInfos;



	const auto &offscreenColorImageView=offscreenRenderTarget->getImageViewByIndex(0);
	const auto &offscreenColorImage = offscreenRenderTarget->getImageByIndex(0);

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout =    VK_IMAGE_LAYOUT_GENERAL;
	postRenderImageSampler=std::make_unique<Sampler>(*device);
	imageInfo.imageView=offscreenColorImageView.getHandle();


	imageInfo.sampler = postRenderImageSampler->getHandle();
	imageInfos[0][1] = imageInfo;

	postDescriptorSet =postDescriptorPool->allocate(*postDescriptorSetLayouts[0], bufferInfos, imageInfos).getHandle();

}



MiniVulkanRenderer::~MiniVulkanRenderer()
{
	Log("Renderer shutting down");

	rayTracingBuilder.reset();

	if(ImGui::GetCurrentContext()!=nullptr)
	{
		ImGui_ImplVulkan_Shutdown();
	}
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}




void MiniVulkanRenderer::calFps()
{
	static double avgFps = 0;

	static double lastTime = 0;
	static double lastFrameCount = 0;
	static auto last = std::chrono::system_clock::now();
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> dur = now - last;
	double frameTime = double(dur.count())*0.001;
	double fps = 1.0 / frameTime;
	last = now;

	frameCount++;
		lastFrameCount++;

	std::string title = "miniVulkanRenderer2 avg fps:";


	lastTime += frameTime;

	if(lastTime>=1.0)
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