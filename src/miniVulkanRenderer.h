#pragma once
#include <iostream>


#include "Common/common.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_ZERO_TO_ONE
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Common/miniLog.h"
#include "Common/camera.h"

#include "Platform/GUIWindow.h"

#include "Rendering/renderContext.h"
#include "Rendering/postQuad.h"

#include "ResourceManagement/ResourceManager.h"


#include "Vulkan/instance.h"
#include "Vulkan/physicalDevice.h"
#include "Vulkan/deviceMemory.h"
#include "Vulkan/device.h"
#include "Vulkan/imageView.h"
#include "Vulkan/image.h"
#include "Vulkan/swapchain.h"
#include "Vulkan/shaderModule.h"
#include "Vulkan/GraphicPipeline.h"
#include "Vulkan/commandPool.h"
#include "Vulkan/commandBuffer.h"
#include "Vulkan/framebuffer.h"
#include "Vulkan/fence.h"
#include "Vulkan/semaphore.h"
#include "Vulkan/descriptorSetLayout.h"
#include "Vulkan/descriptorPool.h"
#include "Vulkan/descriptorSet.h"
#include "Vulkan/buffer.h"
#include "Vulkan/pipelineLayout.h"
#include "Vulkan/renderPass.h"
#include "Vulkan/rayTracingBuilder.h"
#include "Vulkan/rayTracingPipeline.h"
#include "Vulkan/descriptorSetBindings.h"
#include "Vulkan/image.h"
#include "Vulkan/graphicsPipelineBuilder.h"






//miniVulkanRenderer2
/* a mini renderer
*/

using namespace mini;
class MiniVulkanRenderer
{
public:
	MiniVulkanRenderer();

	~MiniVulkanRenderer();

	// init-----------------

	void load();

	void loadBugBox();

	void loadTestGltf();

	void loadShowCase();

	void loadSponza();

	void loadFeatures();

	void init(int width = 1080, int height = 768);

	void initImGUI();

	void createOffScreenFrameBuffer();

	void initRayTracingRender();


	// init raytracing-------
	void buildRayTracing();
	void createBottomLevelAS();
	void createTopLevelAS();
	void createRtDescriptorSet();
	void updateRtDescriptorSet();
	void createRtPipeline();
	void createRtShaderBindingTable();

	void raytrace(CommandBuffer& cmd, const glm::vec4& clearColor);

	// ui---------------------
	void renderUI(std::vector<VkClearValue>& clearValues);

	//post -----------------
	void initPostRender();
	void updatePostDescriptorSet();

	void resetFrame();
	void updateFrame();

	void loop();

	void updateInstances();

	void rasterize(CommandBuffer& cmd);

	void processIO();

	void keyControl();

	void mouseControl();

	void joystickControl();

	static void mouseCallBack(GLFWwindow* window, double xpos, double ypos);

	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	static void joystickCallback(int jid, int event);

	static void dropCallback(GLFWwindow* window, int count, const char** path);

	void cleanScene();

	void calFps();

	void handleSizeChange();

	Camera& getCamera();

	auto objModelToVkGeometryKHR(const ObjModel& model);

	PushConstantRaster pcRaster{
		glm::identity<mat4>(),                 // 
		{10.f, 15.f, 8.f},   // light position
		0,                   // instance id
		0.5f,               // light intensity
		0,                  // light type
		0,                   // skyLightIntensity
	};

	PushConstantRay pcRay{
		{},  // clearColor
		{},  // lightPosition
		0,   // lightIntensity
		0,   // lightType
		0,   // frame
		1,   // nbSample
		5,   // maxDepth
		0,   // pbrMode
		0,   // debugMode
		1,   // skyLightIntensity
		0,   // totalFrameCount
	};

	PushConstantPost pcPost{
		1,   // exposure
	};

private:
	bool canRaytracing = false;
	bool useRaytracing = true;
	int width, height;
	VkExtent2D surfaceExtent{};

	unsigned long long frameCount = 0;

	int maxFrames{ 10000 };
	const int MAX_FRAMES_LIMIT = { 10000 };


	VkFormat defaultSurfaceColorFormat = VK_FORMAT_R8G8B8A8_SRGB;
	VkFormat defaultSurfaceDepthFormat = VK_FORMAT_X8_D24_UNORM_PACK32;

	VkFormat offscreenColorFormat{ VK_FORMAT_R32G32B32A32_SFLOAT };
	VkFormat offscreenDepthFormat{ VK_FORMAT_X8_D24_UNORM_PACK32 };


	std::unique_ptr<Instance>        instance;
	std::unique_ptr<GUIWindow>       window;
	std::shared_ptr<PhysicalDevice>  physicalDevice;
	std::unique_ptr<Device>          device;
	VkSurfaceKHR surface{};

	std::unique_ptr<ResourceManager> resourceManager;

	std::unique_ptr<RenderContext> renderContext;

	// ImGui data
	std::unique_ptr<DescriptorPool> imguiDescPool;

	std::unique_ptr<CommandPool> tempCommandPool;

	// offscreen render data
	std::unique_ptr<RenderTarget> offscreenRenderTarget;
	std::unique_ptr<FrameBuffer> offscreenFramebuffer;



	std::unique_ptr<GraphicsPipelineBuilder> graphicsPipelineBuilder;







	// Raytracing pipeline data
	VkPhysicalDeviceRayTracingPipelinePropertiesKHR    rtProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR };
	std::unique_ptr<RayTracingBuilder>                 rayTracingBuilder;
	DescriptorSetBindings                              rtDescSetBindings;
	std::vector<std::unique_ptr<ShaderModule>>         rtShaderModules;
	std::unique_ptr<DescriptorPool>                    rtDescPool;
	std::unique_ptr<DescriptorSetLayout>               rtDescSetLayout;
	VkDescriptorSet                                    rtDescSet;
	std::vector<VkRayTracingShaderGroupCreateInfoKHR>  rtShaderGroups;
	std::unique_ptr<PipelineLayout>                    rtPipelineLayout;
	std::unique_ptr<RayTracingPipeline>                rtPipeline;

	std::unique_ptr<Buffer>                            rtSBTBuffer;
	VkStridedDeviceAddressRegionKHR                    rgenRegion{};
	VkStridedDeviceAddressRegionKHR                    missRegion{};
	VkStridedDeviceAddressRegionKHR                    hitRegion{};
	VkStridedDeviceAddressRegionKHR                    callRegion{};
	std::vector<VkAccelerationStructureInstanceKHR>    tlas;
	VkBuildAccelerationStructureFlagsKHR               rtFlags;



	// post pipeline data
	std::vector<std::unique_ptr<ShaderModule>>         postShaderModules;
	std::vector<std::shared_ptr<DescriptorSetLayout>>  postDescriptorSetLayouts;
	DescriptorSetBindings                              postDescSetBind;
	std::unique_ptr<DescriptorPool>                    postDescriptorPool;
	VkDescriptorSet                                    postDescriptorSet;
	std::unique_ptr<PipelineLayout>                    postPipelineLayout;
	std::unique_ptr<RenderPass>                        postRenderPass;
	std::unique_ptr<GraphicPipeline>                   postPipeline;
	std::unique_ptr<PostQuad>                          postQuad;
	std::unique_ptr<Sampler>                           postRenderImageSampler;

	Camera  camera;


};

extern MiniVulkanRenderer miniRenderer;