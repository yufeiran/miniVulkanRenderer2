


#include "miniVulkanRenderer.h"
#include <chrono>
#include "Vulkan/shaderInfo.h"
#include "ResourceManagement/ResourceManager.h"
#include "Vulkan/sampler.h"



#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"


using namespace mini;
using namespace std::chrono;

MiniVulkanRenderer::MiniVulkanRenderer()
{
	volkInitialize();
}

void MiniVulkanRenderer::init(int width, int height)
{
	LogLogo();
	Log("init start");
	LogTimerStart("init");

	width = width;
	height = height;
	window = std::make_unique<GUIWindow>(width, height, "miniVulkanRenderer2");
	window->setMouseCallBack(mouseCallBack);
	window->setJoystickCallBack(joystickCallback);
	window->setMouseButtonCallBack(mouseButtonCallbcak);

	LogSpace();
	instance = std::make_unique<Instance>();

	surface = window->createSurface(instance->getHandle());

	auto& gpu = instance->getFirstGpu();

	physicalDevice=std::make_shared<PhysicalDevice>(gpu);

	defaultSurfaceDepthFormat =gpu.findDepthFormat();

	std::vector<const char*> deviceExtension = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME ,
		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
		VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME
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
	

	glm::mat4 objMat = glm::mat4(1.0f);
	objMat = glm::translate(objMat,{2,0,0});
	objMat = glm::scale(objMat,{5,5,5});
	resourceManager->loadObjModel("bunny", "../../assets/bunny/bunny.obj",objMat);

	objMat = glm::mat4(1.0f);
	objMat = glm::translate(objMat,{4,2.5,0});
	objMat = glm::scale(objMat,{2,2,2});
	resourceManager->loadObjModel("smpl", "../../assets/smpl/smpl.obj",objMat);

	resourceManager->loadObjModel("plane", "../../assets/plane/plane.obj");


	objMat = glm::mat4(1.0f);
	objMat = glm::translate(objMat,{0,2,0});
	objMat = glm::scale(objMat,{1,1,1});
	//resourceManager->loadObjModel("backpack", "../../assets/backpack/backpack.obj",objMat, true);

	resourceManager->loadScene("../../assets/glTFBox/Box.gltf",objMat);
	
	//resourceManagement->loadModel("Medieval_building", "../../assets/nv_raytracing_tutorial_scene/Medieval_building.obj",true);

	//resourceManagement->loadModel("plane", "../../assets/nv_raytracing_tutorial_scene/plane.obj",true);

	std::vector<std::string> defaultCubeMapNames={
		"../../assets/skybox/default/right.jpg",
		"../../assets/skybox/default/left.jpg",
		"../../assets/skybox/default/top.jpg",
		"../../assets/skybox/default/bottom.jpg",
		"../../assets/skybox/default/front.jpg",
		"../../assets/skybox/default/back.jpg",
	};

	std::vector<std::string> yokohamaCubeMapNames={
		"../../assets/skybox/Yokohama3/posx.jpg",
		"../../assets/skybox/Yokohama3/negx.jpg",
		"../../assets/skybox/Yokohama3/posy.jpg",
		"../../assets/skybox/Yokohama3/negy.jpg",
		"../../assets/skybox/Yokohama3/posz.jpg",
		"../../assets/skybox/Yokohama3/negz.jpg"
	};
	


	resourceManager->loadCubemap(yokohamaCubeMapNames);



	LogTimerEnd("Load scene");

	LogSpace();

	createDescriptorSetLayout();
	createRasterPipeline();


	createUniformBuffer();

	createObjDescriptionBuffer();

	updateDescriptorSet();

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


	std::vector<std::shared_ptr<DescriptorSetLayout>> layouts{descSetLayout};
	renderContext->prepare(*postRenderPass,*resourceManager,layouts,
		rasterPipeline->getShaderModules().front()->getShaderInfo());


	initImGUI();
	ImGui_ImplGlfw_InitForVulkan(window->getHandle(),true);

	if(canRaytracing)
	{
		LogTimerStart("build AS");
		initRayTracingRender();
		createBottomLevelAS();
		createTopLevelAS();
		LogTimerEnd("build AS");
		createRtDescriptorSet();
		createRtPipeline();
		createRtShaderBindingTable();
	}


	LogSpace();

	window->showWindow();
	LogTimerEnd("init");
}

void MiniVulkanRenderer::initImGUI()
{

	ImGui::CreateContext();
	ImGuiIO& io   = ImGui::GetIO();
	io.IniFilename = nullptr; // Avoiding the INI file
	io.LogFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.FontGlobalScale = 1.5f; 

	ImGuiStyle& style = ImGui::GetStyle();

	ImVec4 TETSU      = ImVec4(38.f / 255.f, 69.f / 255.f, 61.f / 255.f, 0.5f);
	ImVec4 AIMIRUCHA  = ImVec4(15.f / 255.f, 76.f / 255.f, 58.f / 255.f, 1.0f);
	ImVec4 KAMENOZOKI = ImVec4(165.f / 255.f, 222.f / 255.f, 228.f / 255.f, 1.0f);

	// set background color
	style.Colors[ImGuiCol_WindowBg] = TETSU;

	// set text color
	style.Colors[ImGuiCol_Text]     = KAMENOZOKI;

	// set button background color 
	style.Colors[ImGuiCol_Button]   = AIMIRUCHA;


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

auto MiniVulkanRenderer::objModelToVkGeometryKHR(const ObjModel& model)
{
	RayTracingBuilder::BlasInput input;


	VkDeviceAddress vertexAddresss=model.vertexBuffer->getBufferDeviceAddress();
	VkDeviceAddress indexAddress =model.indexBuffer->getBufferDeviceAddress();

	uint32_t maxPrimitiveCount = model.nbIndices/3;

	VkAccelerationStructureGeometryTrianglesDataKHR triangles{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR};
	triangles.vertexFormat			     = VK_FORMAT_R32G32B32_SFLOAT;
	triangles.vertexData.deviceAddress   = vertexAddresss;
	triangles.vertexStride				 = sizeof(Vertex);

	triangles.indexType                  = VK_INDEX_TYPE_UINT32;
	triangles.indexData.deviceAddress    = indexAddress;

	triangles.maxVertex = model.nbVertices - 1;

	VkAccelerationStructureGeometryKHR asGeom{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR};
	asGeom.geometryType       = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	asGeom.flags              = VK_GEOMETRY_OPAQUE_BIT_KHR;
	asGeom.geometry.triangles = triangles;

	VkAccelerationStructureBuildRangeInfoKHR offset={};

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

	const auto& models = resourceManager->getModels();

	for(const auto& model : models)
	{
		auto blas = objModelToVkGeometryKHR(*model);

		allBlas.push_back(blas);
	
	}

	rayTracingBuilder->buildBlas(allBlas,VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
}

void MiniVulkanRenderer::createTopLevelAS()
{
	Log("start createTLAS");


	const auto& instances = resourceManager->getInstances();
	tlas.reserve(instances.size());

	for(const ObjInstance& instance:instances)
	{
		VkAccelerationStructureInstanceKHR rayInst{};
		uint32_t modelId = instance.objIndex;
		rayInst.transform                           = toTransformMatrixKHR(instance.transform);
		rayInst.instanceCustomIndex                 = modelId;
		rayInst.accelerationStructureReference      = rayTracingBuilder->getBlasDeviceAddress(modelId);
		rayInst.flags                               = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		rayInst.mask                                = 0xFF; // Only be hit if rayMask & instance.mask != 0
		rayInst.instanceShaderBindingTableRecordOffset=0;
		tlas.emplace_back(rayInst);
	}
	rtFlags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;

	rayTracingBuilder->buildTlas(tlas,rtFlags);
}

void MiniVulkanRenderer::createRtDescriptorSet()
{
	rtDescSetBindings.addBinding(RtBindings::eTlas,VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,1,
									  VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR); // TLAS
	rtDescSetBindings.addBinding(RtBindings::eOutImage,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1,
									  VK_SHADER_STAGE_RAYGEN_BIT_KHR);   // Output image

	rtDescPool      = rtDescSetBindings.createPool(*device);
	rtDescSetLayout = rtDescSetBindings.createLayout(*device);

	auto descSetLayout = rtDescSetLayout->getHandle();

	VkDescriptorSetAllocateInfo allocateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
	allocateInfo.descriptorPool      = rtDescPool->getHandle();
	allocateInfo.descriptorSetCount  = 1;
	allocateInfo.pSetLayouts         = &descSetLayout;
	vkAllocateDescriptorSets(device->getHandle(),&allocateInfo,&rtDescSet);

	VkAccelerationStructureKHR                         tlas  = rayTracingBuilder->getAccelerationStructure();
	VkWriteDescriptorSetAccelerationStructureKHR       descASInfo{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR};
	descASInfo.accelerationStructureCount = 1;
	descASInfo.pAccelerationStructures    = &tlas;
	VkDescriptorImageInfo imageInfo{{},offscreenRenderTarget->getImageViewByIndex(0).getHandle(),VK_IMAGE_LAYOUT_GENERAL};

	std::vector<VkWriteDescriptorSet> writes;
	writes.emplace_back(rtDescSetBindings.makeWrite(rtDescSet,RtBindings::eTlas,&descASInfo));
	writes.emplace_back(rtDescSetBindings.makeWrite(rtDescSet,RtBindings::eOutImage,&imageInfo));

	vkUpdateDescriptorSets(device->getHandle(),static_cast<uint32_t>(writes.size()),writes.data(),0,nullptr);
}

void MiniVulkanRenderer::updateRtDescriptorSet()
{
	// update output buffer
	const auto &offscreenColorImageView=offscreenRenderTarget->getImageViewByIndex(0);
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout =    VK_IMAGE_LAYOUT_GENERAL;
	imageInfo.imageView=offscreenColorImageView.getHandle();
	imageInfo.sampler = postRenderImageSampler->getHandle();
	VkWriteDescriptorSet writeDescriptorSets = rtDescSetBindings.makeWrite(rtDescSet, RtBindings::eOutImage, &imageInfo);
	vkUpdateDescriptorSets(device->getHandle(), 1, &writeDescriptorSets, 0, nullptr);

}

void MiniVulkanRenderer::createRtPipeline()
{
	enum StageIndices 
	{
		eRaygen,
		eMiss0,
		eMiss1,
		eClosetHit,
		eShaderGroupCount
	};


	ShaderModule rayGenShader("../../spv/raytrace.rgen.spv",*device,VK_SHADER_STAGE_RAYGEN_BIT_KHR);
	ShaderModule rayCHitShader("../../spv/raytrace.rchit.spv",*device,VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	ShaderModule rayMissShader("../../spv/raytrace.rmiss.spv",*device,VK_SHADER_STAGE_MISS_BIT_KHR);
	ShaderModule rayShadowMissShader("../../spv/raytraceShadow.rmiss.spv",*device,VK_SHADER_STAGE_MISS_BIT_KHR);

	// All stages 
	std::vector<VkPipelineShaderStageCreateInfo> stages{};
	stages.resize(eShaderGroupCount);

	VkPipelineShaderStageCreateInfo stage{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
	stage.pName = "main";  // All the same entry point
	// Raygen
	stage.module       = rayGenShader.getHandle();
	stage.stage        = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	stages[eRaygen]    = stage;
	// Miss0 
	stage.module       = rayMissShader.getHandle();
	stage.stage        = VK_SHADER_STAGE_MISS_BIT_KHR;
	stages[eMiss0]     = stage;
	// Miss1 shadow pass 
	stage.module       = rayShadowMissShader.getHandle();
	stage.stage        = VK_SHADER_STAGE_MISS_BIT_KHR;
	stages[eMiss1]     = stage;
	// Hit Group - Closest Hit
	stage.module       = rayCHitShader.getHandle();
	stage.stage        = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	stages[eClosetHit] = stage;

	// Shader groups
	VkRayTracingShaderGroupCreateInfoKHR group{VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR};
	group.anyHitShader       = VK_SHADER_UNUSED_KHR;
	group.closestHitShader   = VK_SHADER_UNUSED_KHR;
	group.generalShader      = VK_SHADER_UNUSED_KHR;
	group.intersectionShader = VK_SHADER_UNUSED_KHR;

	// Raygen
	group.type          = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	group.generalShader = eRaygen;
	rtShaderGroups.push_back(group);

	// Miss0 
	group.type          = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	group.generalShader = eMiss0;
	rtShaderGroups.push_back(group);

	// Miss1 shadow
	group.type          = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	group.generalShader = eMiss1;
	rtShaderGroups.push_back(group);

	// closet hit shader
	group.type             = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
	group.generalShader    = VK_SHADER_UNUSED_KHR;
	group.closestHitShader = eClosetHit;
	rtShaderGroups.push_back(group);

	// Push constant
	VkPushConstantRange pushConstant{VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR ,
									 0, sizeof(PushConstantRay)};
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges    = &pushConstant;

	// Descriptor sets: set 0 for raytracing , set 1 for global scene
	std::vector<VkDescriptorSetLayout> rtDescSetLayouts = {rtDescSetLayout->getHandle(),descSetLayout->getHandle()};
	pipelineLayoutCreateInfo.setLayoutCount             = static_cast<uint32_t> (rtDescSetLayouts.size());
	pipelineLayoutCreateInfo.pSetLayouts                = rtDescSetLayouts.data();

	rtPipelineLayout = std::make_unique<PipelineLayout>(*device,pipelineLayoutCreateInfo);

	// Assemble the shader stages and recursion depth info into the ray tracing pipeline
	VkRayTracingPipelineCreateInfoKHR rayPipelineInfo{VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR};
	rayPipelineInfo.stageCount = static_cast<uint32_t>(stages.size());  // Stages are shaders
	rayPipelineInfo.pStages    = stages.data();

	rayPipelineInfo.groupCount = static_cast<uint32_t>(rtShaderGroups.size());
	rayPipelineInfo.pGroups    = rtShaderGroups.data();

	rayPipelineInfo.maxPipelineRayRecursionDepth = 2; // Ray depth
	rayPipelineInfo.layout                       = rtPipelineLayout->getHandle();
	
	rtPipeline = std::make_unique<RayTracingPipeline>(*device,rayPipelineInfo);
}



void MiniVulkanRenderer::createRtShaderBindingTable()
{
	uint32_t missCount{2};
	uint32_t hitCount{1};
	auto     handleCount = 1 + missCount + hitCount;
	uint32_t handleSize  = rtProperties.shaderGroupHandleSize; 

	uint32_t handleSizeAligned = align_up(handleSize, rtProperties.shaderGroupHandleAlignment);

	rgenRegion.stride = align_up(handleSizeAligned, rtProperties.shaderGroupBaseAlignment); //step size
	rgenRegion.size   = rgenRegion.stride;
	missRegion.stride = handleSizeAligned;
	missRegion.size   = align_up(missCount * handleSizeAligned, rtProperties.shaderGroupBaseAlignment);
	hitRegion.stride  = handleSizeAligned;
	hitRegion.size    = align_up(hitCount * handleSizeAligned, rtProperties.shaderGroupBaseAlignment);

	// Get the shader group handles 
	uint32_t             dataSize = handleCount * handleSize;
	std::vector<uint8_t> handles(dataSize);
	auto result = vkGetRayTracingShaderGroupHandlesKHR(device->getHandle(),rtPipeline->getHandle(), 0, handleCount, dataSize, handles.data());
	assert(result == VK_SUCCESS);

	// Allocate a buffer for storing the SBT
	VkDeviceSize sbtSize = rgenRegion.size + missRegion.size + hitRegion.size + callRegion.size;
	rtSBTBuffer          = std::make_unique<Buffer>(*device,
													sbtSize,
													VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
														| VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR,
													VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	// Find the SBT address of each group
	VkBufferDeviceAddressInfo info{VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, nullptr, rtSBTBuffer->getHandle()};
	VkDeviceAddress           sbtAddress = rtSBTBuffer->getBufferDeviceAddress();
	rgenRegion.deviceAddress             = sbtAddress;
	missRegion.deviceAddress             = sbtAddress + rgenRegion.size;
	hitRegion.deviceAddress              = sbtAddress + rgenRegion.size + missRegion.size;

	// Helper to retrieve the handle data
	auto getHandle = [&] (int i) { return handles.data() + i * handleSize; };

	rtSBTBuffer->persistentMap(sbtSize);
	// Map the SBT buffer and write in the handles.
	auto*     pSBTBuffer = reinterpret_cast<uint8_t*>(rtSBTBuffer->getMapAddress());
	uint8_t*  pData{nullptr};
	uint32_t handleIdx{0};

	// Raygen
	pData = pSBTBuffer;
	memcpy(pData, getHandle(handleIdx++), handleSize);

	// Miss
	pData = pSBTBuffer + rgenRegion.size;
	for(uint32_t c = 0; c < missCount; c++)
	{
		memcpy(pData, getHandle(handleIdx++), handleSize);
		pData += missRegion.stride;
	}

	// Hit 
	pData = pSBTBuffer + rgenRegion.size + missRegion.size;
	for(uint32_t c = 0; c < hitCount; c++)
	{
		memcpy(pData,getHandle(handleIdx++),handleSize);
	}
	rtSBTBuffer->unpersistentMap();

}

void MiniVulkanRenderer::raytrace(CommandBuffer& cmd, const glm::vec4& clearColor)
{
	updateFrame();
	if(pcRay.frame >= maxFrames)
		return;

	// Initializing push constant vulues
	pcRay.clearColor     = clearColor;
	pcRay.lightPosition  = pcRaster.lightPosition;
	pcRay.lightIntensity = pcRaster.lightIntensity;
	pcRay.lightType      = pcRaster.lightType;

	std::vector<VkDescriptorSet> descSets{rtDescSet, descSet};
	vkCmdBindPipeline(cmd.getHandle(),VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,rtPipeline->getHandle());
	vkCmdBindDescriptorSets(cmd.getHandle(),VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipelineLayout->getHandle(), 0,
							(uint32_t)descSets.size(), descSets.data(), 0, nullptr);
	vkCmdPushConstants(cmd.getHandle(), rtPipelineLayout->getHandle(),
		              VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR,
		              0, sizeof(PushConstantRay), &pcRay);

	vkCmdTraceRaysKHR(cmd.getHandle(), &rgenRegion, &missRegion, &hitRegion, &callRegion, surfaceExtent.width,surfaceExtent.height,1);
}

void MiniVulkanRenderer::renderUI(std::vector<VkClearValue>&  clearValues)
{
	static ImGuiTreeNodeFlags_ isLightHeaderOpen = ImGuiTreeNodeFlags_DefaultOpen;
	static ImGuiTreeNodeFlags_ isRenderingHeaderOpen = ImGuiTreeNodeFlags_DefaultOpen;


	bool changed = false;

	ImGui::SetNextWindowSize(ImVec2(500,0));
	ImGui::Begin("Setting");
	changed |= ImGui::ColorEdit3("clearColor",(float*)(&(clearValues[0].color)));
	changed |= ImGui::Checkbox("Ray Tracer mode",&useRaytracing);
	
	if(ImGui::CollapsingHeader("Light",ImGuiTreeNodeFlags_DefaultOpen ))
	{
		auto& pc = pcRaster;
		changed |= ImGui::RadioButton("Point", &pc.lightType, 0);
		ImGui::SameLine();
		changed |= ImGui::RadioButton("Infinite", &pc.lightType,1);

		changed |= ImGui::SliderFloat3("Position", &pc.lightPosition.x, -20.f, 20.f);
		changed |= ImGui::SliderFloat("Intensity", &pc.lightIntensity, 0.f , 150.f);
	}
	if(ImGui::CollapsingHeader("Rendering",ImGuiTreeNodeFlags_DefaultOpen))
	{
		changed |= ImGui::SliderInt("Max Frames", &maxFrames, 1, MAX_FRAMES_LIMIT);
		changed |= ImGui::SliderInt("Sample Number", &pcRay.nbSample, 1, 10);
		ImGui::Text("Now Frame %d ", pcRay.frame);
	}

	
	ImGui::End();



	if(changed)
		resetFrame();
}

void MiniVulkanRenderer::loop()
{
	std::vector<VkClearValue> clearValues(2);
	VkClearColorValue defaultClearColor = {106.0f/256,131.0f/256,114.0f/256,1.0f};
	clearValues[0].color = defaultClearColor;
	clearValues[1].depthStencil = { 1.0f,0 };



	while(!window->shouldClose()){
		calFps();
		processIO();


		updateInstances();

		auto result= renderContext->beginFrame();
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			handleSizeChange();
			continue;
		}



		auto& cmd = renderContext->getCurrentCommandBuffer();
		auto& renderFrame = renderContext->getActiveFrame();

		const auto& swapChainFormat = renderContext->getFormat();

		auto& frameBuffer = renderFrame.getFrameBuffer();

		// ImGui 
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			renderUI(clearValues);
		}
	
		cmd.reset();
		cmd.begin();

		updateUniformBuffer(cmd);

		// Raster render pass
		{
			

			if(useRaytracing)
			{
				glm::vec4 clearColor = {clearValues[0].color.float32[0],
										clearValues[0].color.float32[1],
										clearValues[0].color.float32[2],
										clearValues[0].color.float32[3]};
				raytrace(cmd,clearColor);
			}
			else 
			{
				cmd.beginRenderPass(*rasterRenderPass, *offscreenFramebuffer,clearValues);
				rasterize(cmd);
				cmd.endRenderPass();
			}

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
		renderContext->submit(device->getGraphicQueue(), &cmd);
		renderContext->endFrame();

	}
	device->waitIdle();
}

auto start = std::chrono::system_clock::now();
void MiniVulkanRenderer::updateInstances()
{
	auto now =std::chrono::system_clock::now();
	std::chrono::duration<float> diff = now - start;
	start = now;
	auto bunnyId                      = resourceManager->getInstanceId("bunny");
	auto & instances                  = resourceManager->getInstances();
	const float deltaAngle            =  6.28318530718f;
	const float offset                = diff.count() * 0.5;
	auto& transform                   = instances[bunnyId].transform;
	transform =  glm::rotate(transform,offset * deltaAngle,{0,1,0});

	VkAccelerationStructureInstanceKHR& tinst = tlas[bunnyId];
	tinst.transform                           = toTransformMatrixKHR(transform);

	rayTracingBuilder->buildTlas(tlas, rtFlags, true);
}

void MiniVulkanRenderer::rasterize(CommandBuffer& cmd)
{
	VkDeviceSize offset{0};

	cmd.setViewPortAndScissor(surfaceExtent);

	cmd.bindPipeline(*rasterPipeline);
	
	cmd.bindDescriptorSet(descSet);

	for(const ObjInstance& inst:resourceManager->instances)
	{
		auto& model          = resourceManager->objModel[inst.objIndex];
		pcRaster.objIndex    = inst.objIndex;
		pcRaster.modelMatrix = inst.transform;
		cmd.pushConstant(pcRaster,static_cast<VkShaderStageFlagBits>( VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT));
		
		cmd.bindVertexBuffer(*model->vertexBuffer);
		cmd.bindIndexBuffer(*model->indexBuffer);
		vkCmdDrawIndexed(cmd.getHandle(),model->nbIndices,1,0,0,0);

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
	resetFrame();
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

	
	createOffScreenFrameBuffer();
	updateRtDescriptorSet();
	updatePostDescriptorSet();



	//rasterPipeline.reset();
 //   rasterPipeline = std::make_unique<GraphicPipeline>(rasterShaderModules,*rasterPipelineLayout,*device,surfaceExtent);
	//rasterPipeline->build(*rasterRenderPass);

	std::vector<std::shared_ptr<DescriptorSetLayout>> layouts{descSetLayout};
	renderContext->prepare(*postRenderPass,*resourceManager,layouts
	, rasterPipeline->getShaderModules().front()->getShaderInfo());

	

}

Camera& MiniVulkanRenderer::getCamera()
{
	return camera;
}

void MiniVulkanRenderer::createOffScreenFrameBuffer()
{
	auto imageColor = Image(*device,surfaceExtent,offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_STORAGE_BIT);
	offscreenRenderTarget=RenderTarget::DEFAULT_CREATE_FUNC(std::move(imageColor));
	offscreenFramebuffer=std::make_unique<FrameBuffer>(*device,*offscreenRenderTarget,*rasterRenderPass);

}


void MiniVulkanRenderer::initRayTracingRender()
{
	// Requesting ray tracing properties
	VkPhysicalDeviceProperties2 prop2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
	prop2.pNext = &rtProperties;
	vkGetPhysicalDeviceProperties2(physicalDevice->getHandle(),&prop2);
	rayTracingBuilder=std::make_unique<RayTracingBuilder>(*device,device->getGraphicQueue().getIndex());


}


void MiniVulkanRenderer::initPostRender()
{
	// create post pipeline!

	ShaderInfo postShaderInfo;
	postShaderInfo.bindingInfoMap[0][1]=BindingInfo{TEXTURE_BINDING_TYPE,DIFFUSE};

	postShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/post.vert.spv",*device,VK_SHADER_STAGE_VERTEX_BIT));
	postShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/post.frag.spv",*device,VK_SHADER_STAGE_FRAGMENT_BIT));

	for(auto&s:postShaderModules)
	{
		s->setShaderInfo(postShaderInfo);
	}

	std::vector<VkPushConstantRange> pushConstants;

	postDescSetBind.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

	postDescriptorSetLayouts.push_back(std::move(postDescSetBind.createLayout(*device)));

	postPipelineLayout = std::make_unique<PipelineLayout>(*device,postDescriptorSetLayouts,pushConstants);

	postRenderPass = std::make_unique<RenderPass>(*device,defaultSurfaceColorFormat);

	
	surfaceExtent=renderContext->getSurfaceExtent();
	auto swapChainFormat = renderContext->getFormat();
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

void MiniVulkanRenderer::updatePostDescriptorSet()
{
	const auto &offscreenColorImageView=offscreenRenderTarget->getImageViewByIndex(0);
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout =    VK_IMAGE_LAYOUT_GENERAL;
	imageInfo.imageView=offscreenColorImageView.getHandle();
	imageInfo.sampler = postRenderImageSampler->getHandle();
	VkWriteDescriptorSet writeDescriptorSets = postDescSetBind.makeWrite(postDescriptorSet, 1, &imageInfo);
	vkUpdateDescriptorSets(device->getHandle(), 1, &writeDescriptorSets, 0, nullptr);

}

void MiniVulkanRenderer::resetFrame()
{
	pcRay.frame = -1;
}

void MiniVulkanRenderer::updateFrame()
{
	static glm::mat4 refCamMat;
	static float  refFov = camera.getFov();

	const auto & m = camera.getViewMat();
	const auto fov = camera.getFov();

	if(memcmp(&refCamMat[0][0], &m[0][0], sizeof(glm::mat4)) != 0 ||  refFov != fov)
	{
		resetFrame();
		refCamMat = m;
		refFov    = fov;
	}
	pcRay.frame++;
}

void MiniVulkanRenderer::createDescriptorSetLayout()
{
	auto nbTxt = static_cast<uint32_t>(resourceManager->textures.size());

	// Camera matrices 
	descSetBindings.addBinding(SceneBindings::eGlobals,  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
											VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
	descSetBindings.addBinding(SceneBindings::eObjDescs, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
		                                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	descSetBindings.addBinding(SceneBindings::eTextures, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nbTxt,
											VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	descSetBindings.addBinding(SceneBindings::eCubeMap,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1,
											VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
											| VK_SHADER_STAGE_MISS_BIT_KHR);

	descSetLayout = descSetBindings.createLayout(*device);
	descPool      = descSetBindings.createPool(*device,1);
	descSet       = descPool->allocateDescriptorSet(*descSetLayout);

}



void MiniVulkanRenderer::createRasterPipeline()
{

	// create raster pipeline !

	rasterShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/vertexShader.vert.spv", *device, VK_SHADER_STAGE_VERTEX_BIT));
	rasterShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/fragmentShader.frag.spv", *device, VK_SHADER_STAGE_FRAGMENT_BIT));
	
	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant={};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);

	//VkDescriptorSetLayoutBinding uboLayoutBinding{};
	//uboLayoutBinding.binding = 0;
	//uboLayoutBinding.descriptorCount = 1;
	//uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	//uboLayoutBinding.pImmutableSamplers = nullptr;

	//VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	//samplerLayoutBinding.binding = 1;
	//samplerLayoutBinding.descriptorCount=1;
	//samplerLayoutBinding.descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	//samplerLayoutBinding.pImmutableSamplers = nullptr;

	//std::vector<VkDescriptorSetLayoutBinding>layoutBindings{uboLayoutBinding,samplerLayoutBinding};



	//rasterDescriptorSetLayouts.push_back(std::make_unique<DescriptorSetLayout>(*device,layoutBindings));

    std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts{descSetLayout};

	rasterPipelineLayout = std::make_unique<PipelineLayout>(*device,descSetLayouts,pushConstants);

	rasterRenderPass     = std::make_unique<RenderPass>(*device,offscreenColorFormat,  VK_IMAGE_LAYOUT_GENERAL);


	surfaceExtent=renderContext->getSurfaceExtent();
	rasterPipeline = std::make_unique<GraphicPipeline>(rasterShaderModules,*rasterPipelineLayout,*device,surfaceExtent);

	rasterPipeline->build(*rasterRenderPass);

}

void MiniVulkanRenderer::createUniformBuffer()
{
	globalsBuffer = std::make_unique<Buffer>(*device,static_cast<VkDeviceSize>(sizeof(GlobalUniforms)),
		static_cast<VkBufferUsageFlags>( VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));

}

void MiniVulkanRenderer::createObjDescriptionBuffer()
{
	objDescBuffer = std::make_unique<Buffer>(*device, resourceManager->objDesc, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
}

void MiniVulkanRenderer::updateDescriptorSet()
{
	std::vector<VkWriteDescriptorSet> writes;

	// Camera matrices and scene description 
	VkDescriptorBufferInfo dbiUnif{globalsBuffer->getHandle(), 0, VK_WHOLE_SIZE};
	writes.emplace_back(descSetBindings.makeWrite(descSet,SceneBindings::eGlobals,&dbiUnif));

	VkDescriptorBufferInfo dbiSceneDesc{objDescBuffer->getHandle(), 0, VK_WHOLE_SIZE};
	writes.emplace_back(descSetBindings.makeWrite(descSet,SceneBindings::eObjDescs,&dbiSceneDesc));

	// All texture samplers 
	std::vector<VkDescriptorImageInfo> diit;
	for(auto& texture : resourceManager->textures)
	{
		diit.emplace_back(texture.descriptor);
	}
	writes.emplace_back(descSetBindings.makeWriteArray(descSet,SceneBindings::eTextures,diit.data()));

	VkDescriptorImageInfo cubeMapInfo{resourceManager->cubeMapTexture.descriptor};
	writes.emplace_back(descSetBindings.makeWrite(descSet,SceneBindings::eCubeMap,&cubeMapInfo));



	vkUpdateDescriptorSets(device->getHandle(),static_cast<uint32_t>(writes.size()),writes.data(),0,nullptr);
}

// update camera matrix
void MiniVulkanRenderer::updateUniformBuffer(CommandBuffer& cmd)
{
	// Prepare new UBO contents on host.
	const float aspectRatio = surfaceExtent.width / static_cast<float>(surfaceExtent.height);
	GlobalUniforms hostUBO = {};
	const auto& view = camera.getViewMat();
	auto& proj = glm::perspective(glm::radians(45.0f), (float)surfaceExtent.width / (float)surfaceExtent.height, 0.1f, 1000.0f);
	proj[1][1] *= -1;

	hostUBO.viewProj    = proj * view;
	hostUBO.viewInverse = glm::inverse(view);
	hostUBO.projInverse = glm::inverse(proj);

	// UBO on the device, and what stages access it.
	VkBuffer deviceUBO      = globalsBuffer->getHandle();
	auto     uboUsageStages = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;

	// set lock to ensure the modified UBO is not visible to previous frames.
	VkBufferMemoryBarrier beforeBarrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
	beforeBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	beforeBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	beforeBarrier.buffer        = deviceUBO;
	beforeBarrier.size          = sizeof(hostUBO);
	vkCmdPipelineBarrier(cmd.getHandle(), uboUsageStages, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_DEPENDENCY_DEVICE_GROUP_BIT, 0,
						nullptr, 1, &beforeBarrier, 0, nullptr);

	vkCmdUpdateBuffer(cmd.getHandle(), globalsBuffer->getHandle(), 0, sizeof(GlobalUniforms),&hostUBO);

	// Making sure the updated UBO will be visible
	VkBufferMemoryBarrier afterBarrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
	afterBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	afterBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	afterBarrier.buffer        = deviceUBO;
	afterBarrier.offset        = 0;
	afterBarrier.size          = sizeof(hostUBO);
	vkCmdPipelineBarrier(cmd.getHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, uboUsageStages, VK_DEPENDENCY_DEVICE_GROUP_BIT, 0,
					    nullptr, 1, &afterBarrier, 0, nullptr);
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