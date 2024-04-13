
#pragma once 

#include "Common/common.h"
#include "Common/light.h"
#include "Vulkan/shaderModule.h"
#include "Vulkan/descriptorSetBindings.h"
#include "Vulkan/descriptorSetLayout.h"
#include "Vulkan/pipelineLayout.h"
#include "Vulkan/renderPass.h"
#include "Vulkan/graphicsPipeline.h"
#include "Rendering/renderContext.h"
#include "Rendering/graphicsRenderPass.h"

namespace mini
{

class Device;
class ResourceManager;
class DescriptorSetBindings;
class DescriptorSetLayout;
class DescriptorPool;


class MakeCubeMapPipeline
{
public:
	MakeCubeMapPipeline(Device& device, ResourceManager& resourceManager,PushConstantRaster& pcRaster);
	~MakeCubeMapPipeline();

	void createHDRToCubeMapUniforms();

	void createDescriptorSetLayout();
	void createRenderTarget();

	void updateDescriptorSet(ImageView& hdrImageView);


	void draw(CommandBuffer& cmd);

	RenderTarget& getRenderTarget() { return *renderTarget; }


private:

	const int CUBE_MAP_SIZE = 4096;
	const VkFormat CUBE_MAP_FORMAT = VK_FORMAT_R8G8B8A8_SRGB ;

	Device&                               device;		
	ResourceManager&                      resourceManager;
	DescriptorSetBindings                 descSetBindings;
	std::shared_ptr<DescriptorSetLayout>  descSetLayout;
	std::unique_ptr<DescriptorPool>       descPool;
	VkDescriptorSet                       descSet;


	std::unique_ptr<PipelineLayout>       pipelineLayout;
	std::unique_ptr<RenderPass>           renderPass;

	std::unique_ptr<RenderTarget>         renderTarget;
	std::unique_ptr<FrameBuffer>          framebuffer;

	std::unique_ptr<HDRToCubeMapRenderPass>  hdrToCubeMapRenderPass;

	PushConstantRaster&                   pcRaster;

	std::unique_ptr<Buffer>               hdrToCubeMapUniforms;
	HDRToCubeMapUniforms				  hdrToCubeMapUniformsData;
};

}