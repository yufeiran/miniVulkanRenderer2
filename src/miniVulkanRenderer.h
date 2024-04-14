#pragma once
#include <iostream>


#include "Common/common.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_ZERO_TO_ONE
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Common/miniLog.h"
#include "Common/camera.h"
#include "Common/light.h"

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
#include "Vulkan/GraphicsPipeline.h"
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
#include "Rendering/rayTracingBuilder.h"
#include "Vulkan/rayTracingPipeline.h"
#include "Vulkan/descriptorSetBindings.h"
#include "Vulkan/image.h"
#include "Rendering/graphicsPipelineBuilder.h"
#include "Rendering/shadowPipelineBuilder.h"
#include "Rendering/ssaoPipelineBuilder.h"
#include "Rendering/pbbloomPipelineBuilder.h"
#include "Rendering/SSRPipelineBuilder.h"
#include "Rendering/makeCubeMapPipeline.h"







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


	void init(int width = 1600, int height = 1080);
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

	void rasterize(CommandBuffer& cmd, VkClearColorValue defaultClearColor);

	// ui---------------------
	void renderUI(std::vector<VkClearValue>& clearValues, VkExtent2D screenSize, bool sizeChange, bool& lightSizeChange);

	bool uiLights(VkExtent2D screenSize, bool sizeChange, bool& lightSizeChange);

	bool uiInstance(VkExtent2D screenSize, bool sizeChange = false);

	//post -----------------
	void initPostRender();
	void updatePostDescriptorSet();

	void resetFrame();
	void updateFrame();

	void loop();


	void updateInstances();


	void processIO();
	void keyControl();
	void mouseControl();
	void joystickControl();
	static void mouseCallBack(GLFWwindow* window, double xpos, double ypos);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void joystickCallback(int jid, int event);
	static void dropCallback(GLFWwindow* window, int count, const char** path);

	void cleanScene();

	void calFps();

	void handleSizeChange();

	Camera& getCamera();

	auto objModelToVkGeometryKHR(const ObjModel& model);

	PushConstantRaster pcRaster{
		glm::identity<mat4>(),                 // 
		0,                   // instance id
		10,                   // skyLightIntensity
		0,                   // debugMode
		1080,				 // width
		768,                 // height
		1,				     // NeedSSAO
		0,                   // lightType 0:normal 1: light
		0,                    // light index
		2,                   // shadowMode  0:normal 1:pcf 2.PCSS
		5,                     // shadowLigheSize
		0.1,                  // ssrStep
		10,                  // ssrMaxDistance
		0.1,                 // ssrEpsilon
		1 			      // ssrAttenuation
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
		10,   // skyLightIntensity
		0,   // totalFrameCount
	};

	PushConstantPost pcPost{
		1,   // exposure
		0,   // debugShadowMap
		0,   // debugBloom
		0,   // debugSSR
		0.020, // pbbloomRadius;
		0.01, // pbbloomIntensity;
		1, // 0:off 1:on
		0.5,  // ssrIntensity
		0   // raytraceMode
	};

	std::vector<Light> lights = {


	};

private:
	bool makeCubeMapEveryTime = false;
	bool canRaytracing = false;
	bool useRaytracing = false;
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


	std::unique_ptr<ShadowPipelineBuilder>   shadowPipelineBuilder;

	std::unique_ptr<SSAOPipelineBuilder>     ssaoPipelineBuilder;

	std::unique_ptr<SSRPipelineBuilder>      ssrPipelineBuilder;

	std::unique_ptr<PBBloomPipelineBuilder>  pbbloomPipelineBuilder;

	std::unique_ptr<MakeCubeMapPipeline>     makeCubeMapPipeline;




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
	std::unique_ptr<GraphicsPipeline>                  postPipeline;
	std::unique_ptr<PostQuad>                          postQuad;
	std::unique_ptr<Sampler>                           postRenderImageSampler;

	Camera  camera;

	void loadBugBox();
	void loadTestGltf();
	void loadShowCase();
	void loadSponza();
	void loadFeatures();



};

extern MiniVulkanRenderer miniRenderer;