#pragma once
#include<iostream>


#include"Common/common.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_ZERO_TO_ONE
#include"glm/vec4.hpp"
#include"glm/mat4x4.hpp"
#include"Common/miniLog.h"
#include"Common/camera.h"

#include"Platform/GUIWindow.h"

#include"Rendering/renderContext.h"

#include"ResourceManagement/resourceManagement.h"
#include"ResourceManagement/postQuad.h"

#include"Vulkan/instance.h"
#include"Vulkan/physicalDevice.h"
#include"Vulkan/deviceMemory.h"
#include"Vulkan/device.h"
#include"Vulkan/imageView.h"
#include"Vulkan/image.h"
#include"Vulkan/swapchain.h"
#include"Vulkan/shaderModule.h"
#include"Vulkan/GraphicPipeline.h"
#include"Vulkan/commandPool.h"
#include"Vulkan/commandBuffer.h"
#include"Vulkan/framebuffer.h"
#include"Vulkan/fence.h"
#include"Vulkan/semaphore.h"
#include"Vulkan/descriptorSetLayout.h"
#include"Vulkan/descriptorPool.h"
#include"Vulkan/descriptorSet.h"
#include"Vulkan/buffer.h"
#include"Vulkan/pipelineLayout.h"
#include"Vulkan/renderPass.h"
#include"Vulkan/rayTracingBuilder.h"



#include"Sprite/sprite.h"
#include"Sprite/spriteList.h"







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
	void init(int width = 1920, int height = 1080);

	void initImGUI();

	void initRayTracing();

	auto modelToVkGeometryKHR(const Model& model);

	void createBottomLevelAS();

	void createTopLevelAS();

	//-----------------------

	void loop();

	void keyControl();

	void mouseControl();

	void joystickControl();

	void drawFrame();

	void recordCommandBuffer(CommandBuffer& cmd, RenderFrame& renderFrame);

	static void mouseCallBack(GLFWwindow* window, double xpos, double ypos);

	static void mouseButtonCallbcak(GLFWwindow* window, int button, int action, int mods);

	static void joystickCallback(int jid, int event);

	void calFps();

	void handleSizeChange();

	Camera& getCamera();

	void createOffScreenFrameBuffer();

	void initRasterRender();

	void initPostRender();

private:

	int width, height;
	VkExtent2D surfaceExtent{};

	unsigned long long frameCount=0;

	VkFormat defaultColorFormat=VK_FORMAT_B8G8R8A8_SRGB;
	VkFormat defaultDepthFormat=VK_FORMAT_X8_D24_UNORM_PACK32;

	
	std::unique_ptr<Instance> instance;

	std::unique_ptr<GUIWindow> window;

	std::unique_ptr<PhysicalDevice> physicalDevice;

	VkSurfaceKHR surface{};

	std::unique_ptr<Device> device;

	std::unique_ptr<RenderContext> renderContext;

	// Raytracing data
	VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR};
	std::unique_ptr<RayTracingBuilder> rayTracingBuilder;

	// ImGui data
	std::unique_ptr<DescriptorPool> imguiDescPool;

	std::unique_ptr<CommandPool> tempCommandPool;

	// offscreen render data
	std::unique_ptr<RenderTarget> offscreenRenderTarget;
	std::unique_ptr<FrameBuffer> offscreenFramebuffer;
	
	VkFormat offscreenColorFormat{VK_FORMAT_B8G8R8A8_SRGB};
	VkFormat offscreenDepthFormat{VK_FORMAT_X8_D24_UNORM_PACK32};

	// raster pipeline data
	std::vector<std::unique_ptr<ShaderModule>>rasterShaderModules;
	std::vector<std::unique_ptr<DescriptorSetLayout>> rasterDescriptorSetLayouts;
	std::unique_ptr<PipelineLayout>rasterPipelineLayout;
	std::unique_ptr<RenderPass>rasterRenderPass;
	std::unique_ptr<GraphicPipeline>rasterPipeline;


	// post pipeline data

	std::vector<std::unique_ptr<ShaderModule>>postShaderModules;
	std::vector<std::unique_ptr<DescriptorSetLayout>> postDescriptorSetLayouts;
	std::unique_ptr<DescriptorPool>postDescriptorPool;
	VkDescriptorSet postDescriptorSet;
	std::unique_ptr<PipelineLayout>postPipelineLayout;
	std::unique_ptr<RenderPass>postRenderPass;
	std::unique_ptr<GraphicPipeline>postPipeline;
	std::unique_ptr<PostQuad> postQuad;
	std::unique_ptr<Sampler> postRenderImageSampler;


	std::unique_ptr<ResourceManagement> resourceManagement;

	SpriteList spriteList;

	Camera  camera;


};

extern MiniVulkanRenderer miniRenderer;