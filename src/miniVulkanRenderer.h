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

	void init(int width = 1920, int height = 1080);

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


private:

	int width, height;  

	unsigned long long frameCount=0;

	VkFormat defaultColorFormat=VK_FORMAT_R32G32B32A32_SFLOAT;
	VkFormat defaultDepthFormat=VK_FORMAT_X8_D24_UNORM_PACK32;

	
	std::unique_ptr<Instance> instance;

	std::unique_ptr<GUIWindow> window;

	std::unique_ptr<PhysicalDevice> physicalDevice;

	VkSurfaceKHR surface{};

	std::unique_ptr<Device> device;

	std::unique_ptr<RenderContext> renderContext;


	// offscreen render data
	std::unique_ptr<FrameBuffer> offscreenFramebuffer;
	std::unique_ptr<Image> offscreenColor;
	std::unique_ptr<Image> offscreenDepth;
	
	VkFormat offscreenColorFormat{VK_FORMAT_R32G32B32A32_SFLOAT};
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
	std::unique_ptr<PipelineLayout>postPipelineLayout;
	std::unique_ptr<RenderPass>postRenderPass;
	std::unique_ptr<GraphicPipeline>postPipeline;

	std::unique_ptr<ResourceManagement> resourceManagement;

	SpriteList spriteList;

	Camera  camera;


};

extern MiniVulkanRenderer miniRenderer;