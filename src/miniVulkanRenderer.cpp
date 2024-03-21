

#include <glm/gtx/matrix_decompose.hpp>
#include "miniVulkanRenderer.h"
#include <chrono>
#include "Vulkan/shaderInfo.h"
#include "ResourceManagement/ResourceManager.h"
#include "Vulkan/sampler.h"
#include <glm/gtx/euler_angles.hpp>




#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"


using namespace mini;
using namespace std::chrono;

void MiniVulkanRenderer::load()
{
	int testCase = 0;
	switch (testCase)
	{
	case 0:
		loadShowCase();
		break;
	case 1:
		loadTestGltf();
		break;
	case 2:
		loadSponza();
		break;
	case 3:
		loadBugBox();
		break;
	case 4:
		loadFeatures();
		break;
	}

}


void MiniVulkanRenderer::loadFeatures()
{
	glm::mat4 objMat = glm::mat4(1.0f);
	objMat = glm::mat4(1.0f);
	//objMat = glm::translate(objMat, glm::vec3(0.0,10.0,0.0));
	//objMat = glm::rotate(objMat,glm::radians(90.0f),glm::vec3(0.0,1.0,0.0));
	//objMat = glm::scale(objMat,{0.01,0.01,0.01});
	//resourceManager->loadScene("D://yufeiran/model/AMD/GI/GI.gltf",objMat);
	//resourceManager->loadScene("D://yufeiran/model/bug/greenballs.gltf",objMat);
	//resourceManager->loadScene("D://yufeiran/model/bug/allwithoutcloth.gltf",objMat);

	//resourceManager->loadScene("D://yufeiran/model/bug/clothbackdrop.gltf",objMat);
	//resourceManager->loadScene("D://yufeiran/model/bug/clothpart1.gltf",objMat);

	// MetalRoughSpheres Test
	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/MetalRoughSpheres/glTF/MetalRoughSpheres.gltf",objMat);

	// TransmissionTest
	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/TransmissionTest/glTF/TransmissionTest.gltf",objMat);
	//BoomBoxWithAxes
	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/BoomBoxWithAxes/glTF/BoomBoxWithAxes.gltf",objMat);

		// Alpha Test
	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/AlphaBlendModeTest/glTF/AlphaBlendModeTest.gltf",objMat);

	// NegativeScaleTest
	// resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/NegativeScaleTest/glTF/NegativeScaleTest.gltf",objMat);

	//NormalTangentMirrorTest
	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/NormalTangentMirrorTest/glTF/NormalTangentMirrorTest.gltf",objMat);

	//resourceManager->loadScene("D://yufeiran/model/bug/bugTangentRect.gltf",objMat);


	//NormalTangentTest 
	resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/NormalTangentTest/glTF/NormalTangentTest.gltf", objMat);


	// TransmissionTest
//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/TransmissionTest/glTF/TransmissionTest.gltf",objMat);

	objMat = glm::mat4(1.0f);
	objMat = glm::translate(objMat, glm::vec3(0.0, 3.0, 0.0));
	objMat = glm::rotate(objMat, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));






	//resourceManager->loadScene("../../assets/lightScene.gltf");

	objMat = glm::mat4(1.0f);
	objMat = glm::translate(objMat, { 0,-1,0 });
	objMat = glm::scale(objMat, { 3,1,3 });
	//resourceManager->loadScene( "../../assets/plane/plane1.gltf",objMat);

}

void MiniVulkanRenderer::loadBugBox()
{
	glm::mat4 objMat = glm::mat4(1.0f);
	objMat = glm::mat4(1.0f);
	//resourceManager->loadScene("D://yufeiran/model/AMD/Deferred/Deferred.gltf",objMat);

	resourceManager->loadScene("D://yufeiran/model/bugbox/bugbox.gltf", objMat);

	resourceManager->loadScene("../../assets/lightScene.gltf");
}

void MiniVulkanRenderer::loadTestGltf()
{
	glm::mat4 objMat = glm::mat4(1.0f);
	objMat = glm::mat4(1.0f);
	objMat = glm::translate(objMat, { 0, 5, -3 });
	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/cube/glTF/cube.gltf", objMat);

	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/BoxVertexColors/glTF/BoxVertexColors.gltf", objMat);

	//
	//resourceManager->loadScene("D://yufeiran/model/AMD/Robot/Robot.gltf");




	objMat = glm::translate(objMat, { 2,0,0 });
	objMat = glm::scale(objMat, { 5,5,5 });
	//resourceManager->loadObjModel("bunny", "../../assets/bunny/bunny.obj",objMat);

	objMat = glm::mat4(1.0f);
	objMat = glm::translate(objMat, { 4,2.5,0 });
	objMat = glm::scale(objMat, { 2,2,2 });
	//resourceManager->loadObjModel("smpl", "../../assets/smpl/smpl.obj",objMat);

	objMat = glm::mat4(1.0f);
	objMat = glm::translate(objMat, { 0,2,0 });
	objMat = glm::scale(objMat, { 1,1,1 });
	//resourceManager->loadObjModel("backpack", "../../assets/backpack/backpack.obj",objMat, true);

	//resourceManager->loadScene("../../assets/glTFBox/Box.gltf",objMat);

	//resourceManager->loadScene("../../assets/glTFBox/Box.gltf");
	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/MetalRoughSpheres/glTF/MetalRoughSpheres.gltf");

	//objMat = glm::mat4(1.0f);
	//objMat = glm::translate(objMat,{-12,5,0});
	//resourceManager->loadScene("../../assets/cornellBox/cornellBox.gltf",objMat);
	objMat = glm::mat4(1.0f);
	objMat = glm::translate(objMat, { 0,0,0 });
	objMat = glm::scale(objMat, { 3,1,3 });
	resourceManager->loadScene("../../assets/plane/plane1.gltf", objMat);


	//resourceManager->loadScene("../../assets/lightScene.gltf");


	objMat = glm::mat4(1.0f);
	objMat = glm::translate(objMat, { 0,3,0 });
	objMat = glm::scale(objMat, { 3,3,3 });
	resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/BoxTextured/glTF/BoxTextured.gltf", objMat);





	//objMat = glm::mat4(1.0f);
	//objMat = glm::translate(objMat,{0, 8, -3});
	////resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/cube/glTF/cube.gltf", objMat);

	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/Duck/glTF/Duck.gltf", objMat);

	//



	//objMat = glm::mat4(1.0f);
	//objMat = glm::translate(objMat,{5, 2, 1});
	//objMat = glm::scale(objMat, {0.005, 0.005, 0.005});


	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/cube/glTF/cube.gltf", objMat);

	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/2CylinderEngine/glTF/2CylinderEngine.gltf", objMat);

	//objMat = glm::mat4(1.0f);
	//objMat = glm::translate(objMat,{-15, 0, 0});
	//objMat = glm::scale(objMat, {0.1, 0.1, 0.1});

	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/GearboxAssy/glTF/GearboxAssy.gltf", objMat);


	//objMat = glm::mat4(1.0f);
	//objMat = glm::translate(objMat,{5, 5, 1});
	//objMat = glm::scale(objMat, {0.01, 0.01, 0.01});
	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/Buggy/glTF/Buggy.gltf",objMat);




	camera.setPos(glm::vec3(-0.0, 0, 15.0));
	camera.setViewDir(-90, 0);
	//pcRaster.lightPosition = glm::vec3(0, 4.5f, 0.f);



	//resourceManagement->loadModel("Medieval_building", "../../assets/nv_raytracing_tutorial_scene/Medieval_building.obj",true);

	//resourceManagement->loadModel("plane", "../../assets/nv_raytracing_tutorial_scene/plane.obj",true);
}

void MiniVulkanRenderer::loadSponza()
{
	glm::mat4 objMat = glm::mat4(1.0f);
	objMat = glm::mat4(1.0f);
	//objMat = glm::translate(objMat,{-10,-1,0});
	//resourceManager->loadScene("D://yufeiran/model/AMD/Deferred/Deferred.gltf",objMat);

	resourceManager->loadScene("D://yufeiran/model/AMD/PBR/PBR.gltf", objMat);

	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf",objMat);

	//resourceManager->loadScene("../../assets/lightScene.gltf");
}



void MiniVulkanRenderer::loadShowCase()
{
	glm::mat4 objMat = glm::mat4(1.0f);
	objMat = glm::mat4(1.0f);
	objMat = glm::scale(objMat, { 0.01, 0.01, 0.01 });

	//objMat = glm::translate(objMat,{0,-3,0});
	//resourceManager->loadScene("D://yufeiran/model/AMD/GI/GI.gltf",objMat);
	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/GearboxAssy/glTF/GearboxAssy.gltf",objMat);
	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/AntiqueCamera/glTF/AntiqueCamera.gltf",objMat);

	objMat = glm::mat4(1.0f);
	objMat = glm::scale(objMat, { 2, 2, 2 });

	objMat = glm::translate(objMat, { 0,1,1 });
	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/BarramundiFish/glTF/BarramundiFish.gltf",objMat);

	//objMat = glm::mat4(1.0f);
	//objMat = glm::translate(objMat,{0,0,3});
	//objMat = glm::scale(objMat, {20, 20, 20});


	//resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/BoomBox/glTF/BoomBox.gltf",objMat);

	//resourceManager->loadScene("../../assets/lightScene.gltf");



	//	objMat = glm::mat4(1.0f);
	//objMat = glm::translate(objMat,{0, 0, -5});
	////resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/cube/glTF/cube.gltf", objMat);

	resourceManager->loadScene("D://yufeiran/model/glTF-Sample-Models/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf", objMat);

	objMat = glm::mat4(1.0f);
	objMat = glm::translate(objMat, { 0,-1,0 });
	objMat = glm::scale(objMat, { 3,1,3 });
	resourceManager->loadScene("../../assets/plane/plane1.gltf", objMat);
}

MiniVulkanRenderer::MiniVulkanRenderer()
{
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



	Log("Load scene");
	LogTimerStart("Load scene");

	resourceManager = std::make_unique<ResourceManager>(*device);

	//resourceManagement->loadModel("BattleCruiser", "../../assets/BattleCruiser/BattleCruiser.obj");

	load();
	//loadSponza();


	std::vector<std::string> defaultCubeMapNames = {
		"../../assets/skybox/default/right.jpg",
		"../../assets/skybox/default/left.jpg",
		"../../assets/skybox/default/top.jpg",
		"../../assets/skybox/default/bottom.jpg",
		"../../assets/skybox/default/front.jpg",
		"../../assets/skybox/default/back.jpg",
	};

	std::vector<std::string> yokohamaCubeMapNames = {
		"../../assets/skybox/Yokohama3/posx.jpg",
		"../../assets/skybox/Yokohama3/negx.jpg",
		"../../assets/skybox/Yokohama3/posy.jpg",
		"../../assets/skybox/Yokohama3/negy.jpg",
		"../../assets/skybox/Yokohama3/posz.jpg",
		"../../assets/skybox/Yokohama3/negz.jpg"
	};



	resourceManager->loadCubemap(defaultCubeMapNames);



	LogTimerEnd("Load scene");

	LogSpace();




	graphicsPipelineBuilder = std::make_unique<GraphicsPipelineBuilder>(*device, *resourceManager, *renderContext, offscreenColorFormat, pcRaster);



	createOffScreenFrameBuffer();

	shadowPipelineBuilder = std::make_unique<ShadowPipelineBuilder>(*device, *resourceManager, pcRaster, graphicsPipelineBuilder->getLightUniformsBuffer());

	ssaoPipelineBuilder = std::make_unique<SSAOPipelineBuilder>(*device, *resourceManager, window->getExtent(), graphicsPipelineBuilder->getDescriptorSetLayout(),
		*offscreenRenderTarget, offscreenColorFormat, pcRaster);

	pbbloomPipelineBuilder = std::make_unique<PBBloomPipelineBuilder>(*device,
		*resourceManager,
		window->getExtent(),
		*offscreenRenderTarget,
		offscreenColorFormat,
		pcPost, 5);


	auto& shadowMapRenderTarget = shadowPipelineBuilder->getDirRenderTarget();
	auto& PointShadowMapRenderPass = shadowPipelineBuilder->getPointRenderTarget();

	graphicsPipelineBuilder->updateDescriptorSet(shadowMapRenderTarget, PointShadowMapRenderPass, *offscreenRenderTarget);





	initPostRender();





	updatePostDescriptorSet();


	std::vector<std::shared_ptr<DescriptorSetLayout>> layouts{ graphicsPipelineBuilder->getDescriptorSetLayout() };
	renderContext->prepare(*postRenderPass, *resourceManager, layouts,
		graphicsPipelineBuilder->getForwardRenderPass().getGraphicsPipeline().getShaderModules().front()->getShaderInfo());


	initImGUI();
	ImGui_ImplGlfw_InitForVulkan(window->getHandle(), true);

	if (canRaytracing)
	{
		buildRayTracing();
	}


	LogSpace();

	window->showWindow();


	addLight(lights, *resourceManager, LIGHT_TYPE_DIRECTIONAL, { 0.f, 20.f, 2.f }, { 0.f, -1.f, 0.f }, { 1.f, 1.f, 1.f }, 2.5f, true);

	LogTimerEnd("init");
}

void MiniVulkanRenderer::initImGUI()
{

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr; // Avoiding the INI file
	io.LogFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.FontGlobalScale = 1.5f;

	ImGuiStyle& style = ImGui::GetStyle();

	ImVec4 TETSU = ImVec4(38.f / 255.f, 69.f / 255.f, 61.f / 255.f, 0.9f);
	ImVec4 AIMIRUCHA = ImVec4(15.f / 255.f, 76.f / 255.f, 58.f / 255.f, 1.0f);
	ImVec4 KAMENOZOKI = ImVec4(165.f / 255.f, 222.f / 255.f, 228.f / 255.f, 1.0f);

	//// set background color
	//style.Colors[ImGuiCol_WindowBg] = TETSU;

	//// set text color
	//style.Colors[ImGuiCol_Text]     = KAMENOZOKI;

	//// set button background color 
	//style.Colors[ImGuiCol_Button]   = AIMIRUCHA;


	imguiDescPool = std::make_unique<DescriptorPool>(*device);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = instance->getHandle();
	init_info.PhysicalDevice = physicalDevice->getHandle();
	init_info.Device = device->getHandle();
	init_info.QueueFamily = device->getGraphicQueue().getFamilyIndex();
	init_info.Queue = device->getGraphicQueue().getHandle();
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = imguiDescPool->getHandle();
	init_info.Subpass = 0;
	init_info.MinImageCount = 2;
	init_info.ImageCount = static_cast<int>(renderContext->getSwapchain().getImageCount());
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.CheckVkResultFn = nullptr;
	init_info.Allocator = nullptr;

	ImGui_ImplVulkan_LoadFunctions(
		[](const char* function_name, void* vulkan_instance) {
			return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance*>(vulkan_instance)), function_name); }, &init_info.Instance);

	ImGui_ImplVulkan_Init(&init_info, postRenderPass->getHandle());

	auto& tempCmd = tempCommandPool->createCommandBuffer();
	tempCmd->beginSingleTime();
	ImGui_ImplVulkan_CreateFontsTexture(tempCmd->getHandle());
	tempCmd->endSingleTime(device->getGraphicQueue());


}

auto MiniVulkanRenderer::objModelToVkGeometryKHR(const ObjModel& model)
{
	RayTracingBuilder::BlasInput input;


	VkDeviceAddress vertexAddresss = model.vertexBuffer->getBufferDeviceAddress();
	VkDeviceAddress indexAddress = model.indexBuffer->getBufferDeviceAddress();

	uint32_t maxPrimitiveCount = model.nbIndices / 3;

	VkAccelerationStructureGeometryTrianglesDataKHR triangles{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR };
	triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
	triangles.vertexData.deviceAddress = vertexAddresss;
	triangles.vertexStride = sizeof(Vertex);

	triangles.indexType = VK_INDEX_TYPE_UINT32;
	triangles.indexData.deviceAddress = indexAddress;

	triangles.maxVertex = model.nbVertices - 1;

	VkAccelerationStructureGeometryKHR asGeom{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR };
	asGeom.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	asGeom.flags = VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;  //Avoid double hits
	asGeom.geometry.triangles = triangles;

	VkAccelerationStructureBuildRangeInfoKHR offset = {};

	offset.firstVertex = 0;
	offset.primitiveCount = maxPrimitiveCount;
	offset.primitiveOffset = 0;
	offset.transformOffset = 0;


	input.asGeometry.emplace_back(asGeom);
	input.asBuildOffsetInfo.emplace_back(offset);

	return input;

}

void MiniVulkanRenderer::buildRayTracing()
{
	tlas.clear();

	LogTimerStart("build AS");
	initRayTracingRender();
	createBottomLevelAS();
	createTopLevelAS();
	LogTimerEnd("build AS");
	createRtDescriptorSet();
	createRtPipeline();
	createRtShaderBindingTable();
}

void MiniVulkanRenderer::createBottomLevelAS()
{
	std::vector<RayTracingBuilder::BlasInput> allBlas;
	//allBlas.reserve(resourceManagement->getModelSum());

	const auto& models = resourceManager->getModels();

	for (const auto& model : models)
	{
		auto blas = objModelToVkGeometryKHR(*model);

		allBlas.push_back(blas);

	}

	rayTracingBuilder->buildBlas(allBlas, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
}

void MiniVulkanRenderer::createTopLevelAS()
{
	//Log("start createTLAS");


	const auto& instances = resourceManager->getInstances();
	tlas.clear();
	tlas.reserve(instances.size());

	for (const ObjInstance& instance : instances)
	{
		VkAccelerationStructureInstanceKHR rayInst{};
		uint32_t modelId = instance.objIndex;

		GltfShadeMaterial& mat = resourceManager->materials[modelId];

		rayInst.transform = toTransformMatrixKHR(instance.transform);
		rayInst.instanceCustomIndex = modelId;
		rayInst.accelerationStructureReference = rayTracingBuilder->getBlasDeviceAddress(modelId);

		VkGeometryInstanceFlagsKHR flags{};
		if (mat.doubleSided == 1)
		{
			flags |= VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		}

		rayInst.flags = flags;
		rayInst.mask = 0xFF; // Only be hit if rayMask & instance.mask != 0
		rayInst.instanceShaderBindingTableRecordOffset = 0;
		tlas.emplace_back(rayInst);
	}
	rtFlags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;

	rayTracingBuilder->buildTlas(tlas, rtFlags);
}

void MiniVulkanRenderer::createRtDescriptorSet()
{
	rtDescSetBindings.addBinding(RtBindings::eTlas, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1,
		VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR); // TLAS
	rtDescSetBindings.addBinding(RtBindings::eOutImage, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1,
		VK_SHADER_STAGE_RAYGEN_BIT_KHR);   // Output image

	rtDescPool = rtDescSetBindings.createPool(*device);
	rtDescSetLayout = rtDescSetBindings.createLayout(*device);

	auto descSetLayout = rtDescSetLayout->getHandle();

	VkDescriptorSetAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	allocateInfo.descriptorPool = rtDescPool->getHandle();
	allocateInfo.descriptorSetCount = 1;
	allocateInfo.pSetLayouts = &descSetLayout;
	vkAllocateDescriptorSets(device->getHandle(), &allocateInfo, &rtDescSet);

	VkAccelerationStructureKHR                         tlas = rayTracingBuilder->getAccelerationStructure();
	VkWriteDescriptorSetAccelerationStructureKHR       descASInfo{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR };
	descASInfo.accelerationStructureCount = 1;
	descASInfo.pAccelerationStructures = &tlas;
	VkDescriptorImageInfo imageInfo{ {},offscreenRenderTarget->getImageViewByIndex(0).getHandle(),VK_IMAGE_LAYOUT_GENERAL };

	std::vector<VkWriteDescriptorSet> writes;
	writes.emplace_back(rtDescSetBindings.makeWrite(rtDescSet, RtBindings::eTlas, &descASInfo));
	writes.emplace_back(rtDescSetBindings.makeWrite(rtDescSet, RtBindings::eOutImage, &imageInfo));

	vkUpdateDescriptorSets(device->getHandle(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

void MiniVulkanRenderer::updateRtDescriptorSet()
{

	VkAccelerationStructureKHR                         tlas = rayTracingBuilder->getAccelerationStructure();
	VkWriteDescriptorSetAccelerationStructureKHR       descASInfo{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR };
	descASInfo.accelerationStructureCount = 1;
	descASInfo.pAccelerationStructures = &tlas;
	//VkDescriptorImageInfo imageInfo{ {},offscreenRenderTarget->getImageViewByIndex(0).getHandle(),VK_IMAGE_LAYOUT_GENERAL };



	// update output buffer
	const auto& offscreenColorImageView = offscreenRenderTarget->getImageViewByIndex(0);
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imageInfo.imageView = offscreenColorImageView.getHandle();
	imageInfo.sampler = postRenderImageSampler->getHandle();
	VkWriteDescriptorSet writeDescriptorSets = rtDescSetBindings.makeWrite(rtDescSet, RtBindings::eOutImage, &imageInfo);
	vkUpdateDescriptorSets(device->getHandle(), 1, &writeDescriptorSets, 0, nullptr);

	std::vector<VkWriteDescriptorSet> writes;
	writes.emplace_back(rtDescSetBindings.makeWrite(rtDescSet, RtBindings::eTlas, &descASInfo));
	writes.emplace_back(rtDescSetBindings.makeWrite(rtDescSet, RtBindings::eOutImage, &imageInfo));

	vkUpdateDescriptorSets(device->getHandle(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);

}

void MiniVulkanRenderer::createRtPipeline()
{
	enum StageIndices
	{
		eRaygen,
		eMiss0,
		eMiss1,
		eClosetHit,
		eAnyHit,
		eShaderGroupCount
	};


	ShaderModule rayGenShader("../../spv/raytrace.rgen.spv", *device, VK_SHADER_STAGE_RAYGEN_BIT_KHR);
	ShaderModule rayCHitShader("../../spv/raytrace.rchit.spv", *device, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	ShaderModule rayAnyHitShader("../../spv/raytrace.rahit.spv", *device, VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
	ShaderModule rayMissShader("../../spv/raytrace.rmiss.spv", *device, VK_SHADER_STAGE_MISS_BIT_KHR);
	ShaderModule rayShadowMissShader("../../spv/raytraceShadow.rmiss.spv", *device, VK_SHADER_STAGE_MISS_BIT_KHR);


	// All stages 
	std::vector<VkPipelineShaderStageCreateInfo> stages{};
	stages.resize(eShaderGroupCount);

	VkPipelineShaderStageCreateInfo stage{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
	stage.pName = "main";  // All the same entry point
	// Raygen
	stage.module = rayGenShader.getHandle();
	stage.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	stages[eRaygen] = stage;
	// Miss0 
	stage.module = rayMissShader.getHandle();
	stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
	stages[eMiss0] = stage;
	// Miss1 shadow pass 
	stage.module = rayShadowMissShader.getHandle();
	stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
	stages[eMiss1] = stage;
	// Hit Group - Closest Hit
	stage.module = rayCHitShader.getHandle();
	stage.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	stages[eClosetHit] = stage;
	// Hit Group - Any Hit
	stage.module = rayAnyHitShader.getHandle();
	stage.stage = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
	stages[eAnyHit] = stage;

	// Shader groups
	VkRayTracingShaderGroupCreateInfoKHR group{ VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR };
	group.anyHitShader = VK_SHADER_UNUSED_KHR;
	group.closestHitShader = VK_SHADER_UNUSED_KHR;
	group.generalShader = VK_SHADER_UNUSED_KHR;
	group.intersectionShader = VK_SHADER_UNUSED_KHR;

	// Raygen
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	group.generalShader = eRaygen;
	rtShaderGroups.push_back(group);

	// Miss0 
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	group.generalShader = eMiss0;
	rtShaderGroups.push_back(group);

	// Miss1 shadow
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	group.generalShader = eMiss1;
	rtShaderGroups.push_back(group);

	// closet hit shader
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
	group.generalShader = VK_SHADER_UNUSED_KHR;
	group.closestHitShader = eClosetHit;
	group.anyHitShader = eAnyHit;
	rtShaderGroups.push_back(group);

	// Push constant
	VkPushConstantRange pushConstant{ VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR ,
									 0, sizeof(PushConstantRay) };
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;

	// Descriptor sets: set 0 for raytracing , set 1 for global scene
	std::vector<VkDescriptorSetLayout> rtDescSetLayouts = { rtDescSetLayout->getHandle(),graphicsPipelineBuilder->getDescriptorSetLayout()->getHandle() };
	pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t> (rtDescSetLayouts.size());
	pipelineLayoutCreateInfo.pSetLayouts = rtDescSetLayouts.data();

	rtPipelineLayout = std::make_unique<PipelineLayout>(*device, pipelineLayoutCreateInfo);

	// Assemble the shader stages and recursion depth info into the ray tracing pipeline
	VkRayTracingPipelineCreateInfoKHR rayPipelineInfo{ VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR };
	rayPipelineInfo.stageCount = static_cast<uint32_t>(stages.size());  // Stages are shaders
	rayPipelineInfo.pStages = stages.data();

	rayPipelineInfo.groupCount = static_cast<uint32_t>(rtShaderGroups.size());
	rayPipelineInfo.pGroups = rtShaderGroups.data();

	rayPipelineInfo.maxPipelineRayRecursionDepth = 31; // Ray depth
	rayPipelineInfo.layout = rtPipelineLayout->getHandle();

	rtPipeline = std::make_unique<RayTracingPipeline>(*device, rayPipelineInfo);
}



void MiniVulkanRenderer::createRtShaderBindingTable()
{
	uint32_t missCount{ 2 };
	uint32_t hitCount{ 1 };
	auto     handleCount = 1 + missCount + hitCount;
	uint32_t handleSize = rtProperties.shaderGroupHandleSize;

	uint32_t handleSizeAligned = align_up(handleSize, rtProperties.shaderGroupHandleAlignment);

	rgenRegion.stride = align_up(handleSizeAligned, rtProperties.shaderGroupBaseAlignment); //step size
	rgenRegion.size = rgenRegion.stride;
	missRegion.stride = handleSizeAligned;
	missRegion.size = align_up(missCount * handleSizeAligned, rtProperties.shaderGroupBaseAlignment);
	hitRegion.stride = handleSizeAligned;
	hitRegion.size = align_up(hitCount * handleSizeAligned, rtProperties.shaderGroupBaseAlignment);

	// Get the shader group handles 
	uint32_t             dataSize = handleCount * handleSize;
	std::vector<uint8_t> handles(dataSize);
	auto result = vkGetRayTracingShaderGroupHandlesKHR(device->getHandle(), rtPipeline->getHandle(), 0, handleCount, dataSize, handles.data());
	assert(result == VK_SUCCESS);

	// Allocate a buffer for storing the SBT
	VkDeviceSize sbtSize = rgenRegion.size + missRegion.size + hitRegion.size + callRegion.size;
	rtSBTBuffer = std::make_unique<Buffer>(*device,
		sbtSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
		| VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	// Find the SBT address of each group
	VkBufferDeviceAddressInfo info{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, nullptr, rtSBTBuffer->getHandle() };
	VkDeviceAddress           sbtAddress = rtSBTBuffer->getBufferDeviceAddress();
	rgenRegion.deviceAddress = sbtAddress;
	missRegion.deviceAddress = sbtAddress + rgenRegion.size;
	hitRegion.deviceAddress = sbtAddress + rgenRegion.size + missRegion.size;

	// Helper to retrieve the handle data
	auto getHandle = [&](int i) { return handles.data() + i * handleSize; };

	rtSBTBuffer->persistentMap(sbtSize);
	// Map the SBT buffer and write in the handles.
	auto* pSBTBuffer = reinterpret_cast<uint8_t*>(rtSBTBuffer->getMapAddress());
	uint8_t* pData{ nullptr };
	uint32_t handleIdx{ 0 };

	// Raygen
	pData = pSBTBuffer;
	memcpy(pData, getHandle(handleIdx++), handleSize);

	// Miss
	pData = pSBTBuffer + rgenRegion.size;
	for (uint32_t c = 0; c < missCount; c++)
	{
		memcpy(pData, getHandle(handleIdx++), handleSize);
		pData += missRegion.stride;
	}

	// Hit 
	pData = pSBTBuffer + rgenRegion.size + missRegion.size;
	for (uint32_t c = 0; c < hitCount; c++)
	{
		memcpy(pData, getHandle(handleIdx++), handleSize);
	}
	rtSBTBuffer->unpersistentMap();

}

void MiniVulkanRenderer::raytrace(CommandBuffer& cmd, const glm::vec4& clearColor)
{
	updateFrame();
	if (pcRay.frame >= maxFrames)
		return;

	// Initializing push constant vulues
	pcRay.clearColor = clearColor;


	std::vector<VkDescriptorSet> descSets{ rtDescSet,graphicsPipelineBuilder->getDescriptorSet() };
	vkCmdBindPipeline(cmd.getHandle(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline->getHandle());
	vkCmdBindDescriptorSets(cmd.getHandle(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipelineLayout->getHandle(), 0,
		(uint32_t)descSets.size(), descSets.data(), 0, nullptr);
	vkCmdPushConstants(cmd.getHandle(), rtPipelineLayout->getHandle(),
		VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR,
		0, sizeof(PushConstantRay), &pcRay);

	vkCmdTraceRaysKHR(cmd.getHandle(), &rgenRegion, &missRegion, &hitRegion, &callRegion, surfaceExtent.width, surfaceExtent.height, 1);
}

void MiniVulkanRenderer::renderUI(std::vector<VkClearValue>& clearValues, VkExtent2D screenSize, bool sizeChange, bool& lightSizeChange)
{
	static ImGuiTreeNodeFlags_ isLightHeaderOpen = ImGuiTreeNodeFlags_DefaultOpen;
	static ImGuiTreeNodeFlags_ isRenderingHeaderOpen = ImGuiTreeNodeFlags_DefaultOpen;

	static int debugModeIndex = 0;

	static bool needSSAO = true;
	bool changed = false;

	ImGui::SetNextWindowSize(ImVec2(500, 0));
	ImGui::Begin("Setting");
	changed |= ImGui::ColorEdit3("clearColor", (float*)(&(clearValues[0].color)));
	changed |= ImGui::Checkbox("Ray Tracer mode", &useRaytracing);


	if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
	{
		changed |= ImGui::SliderInt("Max Frames", &maxFrames, 1, MAX_FRAMES_LIMIT);
		changed |= ImGui::SliderInt("Sample Number", &pcRay.nbSample, 1, 10);
		changed |= ImGui::SliderInt("Max Depth", &pcRay.maxDepth, 1, 20);
		ImGui::Text("Now Frame %d ", pcRay.frame);

		changed |= ImGui::Checkbox("SSAO", &needSSAO);
		if (needSSAO == false)
		{
			pcRaster.needSSAO = 0;
		}
		else {
			pcRaster.needSSAO = 1;
		}


	}
	if(ImGui::CollapsingHeader("Shadow", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const int SHADOW_MODE_SUM = 3;
		const char* SHADOW_MODE_STR[SHADOW_MODE_SUM] = {
			"normal",
			"PCF",
			"PCSS"
		};

		changed |= ImGui::Combo("shadow mode", &pcRaster.shadowMode, SHADOW_MODE_STR, SHADOW_MODE_SUM);
		if(pcRaster.shadowMode == 2)
		{
			changed |= ImGui::SliderFloat("shadow light size", &pcRaster.shadowLightSize, 1, 60);
		}
		
	}

	if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const int DEBUG_MODE_SUM = 12;
		const char* DEBUG_MODE_STR[DEBUG_MODE_SUM] = {
			"no debug",
			"base color",
			"normal",
			"metallic",
			"emissive",
			"alpha",
			"roughness",
			"texcoord",
			"tangent",
			"bitangent",
			"specular",
			"SSAO"
		};
		changed |= ImGui::Combo("mode", &debugModeIndex, DEBUG_MODE_STR, DEBUG_MODE_SUM);
		pcRay.debugMode = debugModeIndex;
		pcRaster.debugMode = debugModeIndex;

	}
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Camera pos:%.1f %.1f %.1f , yaw %.1f pitch %.1f ", camera.getPos()[0], camera.getPos()[1], camera.getPos()[2],
			camera.getYaw(), camera.getPitch());
	}
	if (ImGui::CollapsingHeader("Post", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SliderFloat("exposure", &pcPost.exposure, 0, 10);

		static bool debugShadowMap = pcPost.debugShadowMap;
		ImGui::Checkbox("debugShadowMap", &debugShadowMap);
		pcPost.debugShadowMap = debugShadowMap;

		static bool debugBloom = pcPost.debugBloom;
		ImGui::Checkbox("debugBloom", &debugBloom);
		pcPost.debugBloom = debugBloom;

		ImGui::SliderFloat("bloomRadius", &pcPost.pbbloomRadius, 0, 0.1);
		ImGui::SliderFloat("bloomIntensity", &pcPost.pbbloomIntensity, 0, 0.3);
		static bool bloomMode = pcPost.pbbloomMode;
		ImGui::Checkbox("bloom", &bloomMode);
		pcPost.pbbloomMode = bloomMode;

	}


	ImGui::End();

	changed |= uiLights(screenSize, sizeChange, lightSizeChange);
	changed |= uiInstance(screenSize, sizeChange);



	if (changed)
		resetFrame();
}

bool MiniVulkanRenderer::uiLights(VkExtent2D screenSize, bool sizeChange, bool& lightSizeChange)
{

	bool changed = false;
	static bool init = true;

	int windowWidth = 500;
	int windowHeight = 1000;

	lightSizeChange = false;

	init = sizeChange;

	if (init == true)
	{
		ImGui::SetNextWindowPos(ImVec2(screenSize.width - windowWidth - 50, 50));
		ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
		init = false;

	}

	//ImGui::ShowDemoWindow();

	//ImGui::SetNextWindowSize(ImVec2(700,0));

	ImGui::Begin("Light Setting");


	bool addChecked = ImGui::Button("add");

	if (addChecked)
	{

		addRandomLight(lights, *resourceManager);
		lightSizeChange = true;
	}

	for (int i = 0; i < lights.size(); i++)
	{
		ImGui::PushID(i);

		if (ImGui::CollapsingHeader(("Light" + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_None))
		{
			auto& light = lights[i];

			auto pos = light.getPosition();
			changed |= ImGui::SliderFloat3("Position", &pos.x, -20.f, 20.f);
			light.setPosition(pos);
			int lightInstanceId = light.getInstanceId();
			if (lightInstanceId != -1)
			{
				auto& instance = resourceManager->getInstances()[lightInstanceId];
				instance.translation = pos;
				instance.updateTransformByFactor();
			}



			auto intensity = light.getIntensity();
			changed |= ImGui::SliderFloat("Intensity", &intensity, 0.f, 10.f);
			light.setIntensity(intensity);

			int type = light.getType();
			changed |= ImGui::RadioButton("Point", &type, 0);
			ImGui::SameLine();

			changed |= ImGui::RadioButton("Infinite", &type, 1);
			light.setType(static_cast<LightType>(type));

			auto color = light.getColor();
			changed |= ImGui::ColorEdit3("Color", &color.x);
			light.setColor(color);

			if (ImGui::Button("remove"))
			{
				delLight(lights, *resourceManager, i);
				lightSizeChange = true;
				changed = true;
			}


		}
		ImGui::PopID();

	}


	if (ImGui::CollapsingHeader("Skylight", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Skylight");
		changed |= ImGui::SliderFloat("SkylightIntensity", &pcRay.skyLightIntensity, 0.f, 300.f);
		pcRaster.skyLightIntensity = pcRay.skyLightIntensity;
	}


	ImGui::End();


	return changed;

}

bool MiniVulkanRenderer::uiInstance(VkExtent2D screenSize, bool sizeChange)
{
	bool changed = false;
	static bool init = true;

	init = sizeChange;

	int windowWidth = 800;
	int windowHeight = 400;

	static bool open = false;

	auto& instance = resourceManager->getInstances();

	if (init == true)
	{
		ImGui::SetNextWindowPos(ImVec2(20, screenSize.height - windowHeight - 50));
		ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

		init = false;
		if (instance.size() < 10)
		{
			open = true;

		}

	}

	ImGui::Begin("Instance", &open);


	for (int i = 0; i < instance.size(); i++)
	{
		ImGui::PushID(i);
		auto& inst = instance[i];
		if (ImGui::CollapsingHeader((std::to_string(i) + "_" + inst.name).c_str(), ImGuiTreeNodeFlags_None))
		{


			/*	glm::mat4& transform = inst.transform;
				glm::vec3 scale;
				glm::quat rotation;
				glm::vec3 translation;
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::decompose(transform,scale,rotation,translation,skew,perspective);

				glm::mat4 rotationMat =  glm::mat4_cast(rotation);
				glm::vec3 eular =  glm::eulerAngles(rotation);*/

				//Log("pitch "+ std::to_string(glm::degrees(glm::pitch(rotation))) + 
				//	" yaw " + std::to_string(glm::degrees(glm::yaw(rotation))) + 
				//	" roll " + std::to_string(glm::degrees(glm::roll(rotation))));

			changed |= ImGui::SliderFloat3("Translation", &inst.translation.x, -20.f, 20.f);

			changed |= ImGui::SliderFloat3("Scale", &inst.scale.x, 0.1f, 10.f);



			changed |= ImGui::SliderAngle("rotateX", &inst.rotation.x);
			changed |= ImGui::SliderAngle("rotateY", &inst.rotation.y);
			changed |= ImGui::SliderAngle("rotateZ", &inst.rotation.z);

			inst.updateTransformByFactor();
			if (inst.type == INSTANCE_TYPE_LIGHT_CUBE)
			{
				int lightID = inst.lightIndex;
				if (inst.lightIndex != -1)
				{
					lights[lightID].setPosition(inst.translation);
				}


			}







			//glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f),scale);


			////glm::mat4 rotationMatX = glm::rotate(glm::mat4(1.0f),eular.x,glm::vec3(1,0,0));
			////glm::mat4 rotationMatY = glm::rotate(glm::mat4(1.0f),eular.y,glm::vec3(0,1,0));
			////glm::mat4 rotationMatZ = glm::rotate(glm::mat4(1.0f),eular.z,glm::vec3(0,0,1));

			////rotationMat = rotationMatX * rotationMatY * rotationMatZ;

			//rotationMat = glm::eulerAngleXYZ(eular.x,eular.y,eular.z);
			//
			//glm::mat4 translationMat = glm::translate(glm::mat4(1.0f),translation);

			//transform = translationMat * rotationMat * scaleMat;


		}
		ImGui::PopID();

	}

	ImGui::End();

	return changed;
}

void MiniVulkanRenderer::loop()
{
	std::vector<VkClearValue> clearValues(3);
	VkClearColorValue defaultClearColor = { 106.0f / 256,131.0f / 256,114.0f / 256,1.0f };
	clearValues[0].color = defaultClearColor;
	clearValues[1].depthStencil = { 1.0f,0 };
	clearValues[2].depthStencil = { 1.0f,0 };

	static bool sizeChange = true;
	static bool lightSizeChange = true;


	while (!window->shouldClose()) {
		calFps();
		processIO();



		if (lightSizeChange)
		{
			LogTimerStart("rebuild RT_TLAS");
			createTopLevelAS();
			updateRtDescriptorSet();
			LogTimerEnd("rebuild RT_TLAS");
			lightSizeChange = false;
			continue;

		}
		updateInstances();

		auto result = renderContext->beginFrame();
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			handleSizeChange();
			sizeChange = true;
			continue;
		}



		pcRaster.screenHeight = window->getExtent().height;
		pcRaster.screenWidth = window->getExtent().width;

		auto& cmd = renderContext->getCurrentCommandBuffer();
		auto& renderFrame = renderContext->getActiveFrame();

		const auto& swapChainFormat = renderContext->getFormat();

		auto& frameBuffer = renderFrame.getFrameBuffer();

		// ImGui 
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			renderUI(clearValues, window->getExtent(), sizeChange, lightSizeChange);
		}


		cmd.reset();
		cmd.begin();

		surfaceExtent = renderContext->getSwapchain().getExtent();



		graphicsPipelineBuilder->update(cmd, camera, surfaceExtent, lights);


		// Raster render pass
		{


			if (useRaytracing)
			{
				glm::vec4 clearColor = { clearValues[0].color.float32[0],
										clearValues[0].color.float32[1],
										clearValues[0].color.float32[2],
										clearValues[0].color.float32[3] };
				raytrace(cmd, clearColor);
			}
			else
			{



				rasterize(cmd, defaultClearColor);

			}

		}

		// Offscreen render pass
		{
			pbbloomPipelineBuilder->draw(cmd);

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

			ImGui::Render();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd.getHandle());

			cmd.endRenderPass();
		}

		cmd.end();
		renderContext->submit(device->getGraphicQueue(), &cmd);
		renderContext->endFrame();

		sizeChange = false;


	}
	device->waitIdle();
}


auto start = std::chrono::system_clock::now();
void MiniVulkanRenderer::updateInstances()
{

	//int lightId =resourceManager->getInstanceId("LightCube");
	auto& instances = resourceManager->getInstances();
	//if(lightId!= -1)
	//{
	//	

	//	auto& lightInstance = instances[lightId];

	//	auto& light = lights[0];

	//	lightInstance.transform = glm::translate(glm::mat4(1.0f),light.getPosition());
	//}





	if (useRaytracing == false)
	{
		return;
	}
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<float> diff = now - start;
	start = now;


	for (int i = 0; i < instances.size(); i++)
	{
		auto& inst = instances[i];
		VkAccelerationStructureInstanceKHR& tinst = tlas[i];
		tinst.transform = toTransformMatrixKHR(inst.transform);
	}

	rayTracingBuilder->buildTlas(tlas, rtFlags, true);
}

void MiniVulkanRenderer::rasterize(CommandBuffer& cmd, VkClearColorValue defaultClearColor)
{
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



	//ssaoPipelineBuilder->draw(cmd,graphicsPipelineBuilder->getDescriptorSet());


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

	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
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
	app->cleanScene();
	const char* filename_ = path[0];
	std::string filename = filename_;
	auto offset = filename.find_last_of('.');
	std::string extName = filename.substr(offset);
	if (extName == ".gltf")
	{
		app->resourceManager->loadScene(filename);
	}
	app->buildRayTracing();



}

void MiniVulkanRenderer::cleanScene()
{
	resetFrame();
	device->waitIdle();



	renderContext.reset();
	renderContext = std::make_unique<RenderContext>(*device, surface, *window);

	surfaceExtent = renderContext->getSurfaceExtent();

	resourceManager.reset();
	resourceManager = std::make_unique<ResourceManager>(*device);


	createOffScreenFrameBuffer();
	updateRtDescriptorSet();
	updatePostDescriptorSet();


	auto& descSetLayout = graphicsPipelineBuilder->getDescriptorSetLayout();
	auto& rasterPipeline = graphicsPipelineBuilder->getForwardRenderPass().getGraphicsPipeline();
	std::vector<std::shared_ptr<DescriptorSetLayout>> layouts{ descSetLayout };
	renderContext->prepare(*postRenderPass, *resourceManager, layouts
		, rasterPipeline.getShaderModules().front()->getShaderInfo());
}


void MiniVulkanRenderer::handleSizeChange()
{
	resetFrame();
	device->waitIdle();

	auto extent = window->getExtent();
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

	surfaceExtent = renderContext->getSurfaceExtent();


	createOffScreenFrameBuffer();

	if (canRaytracing == true)
	{
		updateRtDescriptorSet();
	}


	updatePostDescriptorSet();

	graphicsPipelineBuilder->rebuild(extent);
	graphicsPipelineBuilder->updateDescriptorSet(shadowPipelineBuilder->getDirRenderTarget(), shadowPipelineBuilder->getPointRenderTarget(), *offscreenRenderTarget);

	pbbloomPipelineBuilder->rebuild(extent, *offscreenRenderTarget, 5);

	updatePostDescriptorSet();



	//rasterPipeline.reset();
 //   rasterPipeline = std::make_unique<GraphicPipeline>(rasterShaderModules,*rasterPipelineLayout,*device,surfaceExtent);
	//rasterPipeline->build(*rasterRenderPass);

	auto& descSetLayout = graphicsPipelineBuilder->getDescriptorSetLayout();
	auto& rasterPipeline = graphicsPipelineBuilder->getForwardRenderPass().getGraphicsPipeline();
	std::vector<std::shared_ptr<DescriptorSetLayout>> layouts{ descSetLayout };
	renderContext->prepare(*postRenderPass, *resourceManager, layouts
		, rasterPipeline.getShaderModules().front()->getShaderInfo());



}

Camera& MiniVulkanRenderer::getCamera()
{
	return camera;
}

void MiniVulkanRenderer::createOffScreenFrameBuffer()
{
	surfaceExtent = window->getExtent();

	std::vector<Image> images;

	auto imageColor = Image(*device, surfaceExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT);


	auto depthFormat = imageColor.getDevice().getPhysicalDevice().findDepthFormat();
	std::unique_ptr<Image> depthImage = std::make_unique<Image>(imageColor.getDevice(),
		imageColor.getExtent(), depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);


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


void MiniVulkanRenderer::initRayTracingRender()
{
	if (!rayTracingBuilder)
	{
		rayTracingBuilder.reset();
	}
	// Requesting ray tracing properties
	VkPhysicalDeviceProperties2 prop2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
	prop2.pNext = &rtProperties;
	vkGetPhysicalDeviceProperties2(physicalDevice->getHandle(), &prop2);
	rayTracingBuilder = std::make_unique<RayTracingBuilder>(*device, device->getGraphicQueue().getIndex());


}


void MiniVulkanRenderer::initPostRender()
{
	// create post pipeline!

	ShaderInfo postShaderInfo;
	postShaderInfo.bindingInfoMap[0][1] = BindingInfo{ TEXTURE_BINDING_TYPE,DIFFUSE };

	postShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/post.vert.spv", *device, VK_SHADER_STAGE_VERTEX_BIT));
	postShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/post.frag.spv", *device, VK_SHADER_STAGE_FRAGMENT_BIT));

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



	postDescriptorSetLayouts.push_back(std::move(postDescSetBind.createLayout(*device)));

	postPipelineLayout = std::make_unique<PipelineLayout>(*device, postDescriptorSetLayouts, pushConstants);

	std::vector<Attachment> attachments;
	{
		Attachment colorAttachment{ defaultSurfaceColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
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


	std::vector<VkDescriptorSetLayoutBinding>postLayoutBindings{ postSamplerLayoutBinding,postSamplerShadowMapLayoutBinding,postSamplerBloomLayoutBinding };

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
	shadowMapInfo.sampler = resourceManager->getDefaultSampler().getHandle();

	writes.push_back(postDescSetBind.makeWrite(postDescriptorSet, 2, &shadowMapInfo));



	VkDescriptorImageInfo imageBloomInfo{};
	imageBloomInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imageBloomInfo.imageView = pbbloomPipelineBuilder->getRenderTargets()[0]->getImageViewByIndex(0).getHandle();
	imageBloomInfo.sampler = postRenderImageSampler->getHandle();

	writes.push_back(postDescSetBind.makeWrite(postDescriptorSet, 3, &imageBloomInfo));



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
}



MiniVulkanRenderer::~MiniVulkanRenderer()
{
	Log("Renderer shutting down");

	rayTracingBuilder.reset();
	graphicsPipelineBuilder.reset();


	if (ImGui::GetCurrentContext() != nullptr)
	{
		ImGui_ImplVulkan_Shutdown();
	}
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	resourceManager.reset();
	offscreenRenderTarget.reset();


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

	VkExtent2D extent = window->getExtent();

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