#pragma once 

#include "Common/common.h"
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


class ShadowPipelineBuilder
{
public:
	ShadowPipelineBuilder(Device& device, ResourceManager& resourceManager,PushConstantRaster& pcRaster);
	~ShadowPipelineBuilder();

	void createDescriptorSetLayout();
	void createRenderTarget();

	RenderTarget& getRenderTarget() { return *renderTarget; }

	void draw(CommandBuffer& cmd);


private:

	const int SHADOW_WIDTH = 1024;
	const int SHADOW_HEIGHT = 1024;

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

	std::unique_ptr<ShadowMapRenderPass>  shadowMapRenderPass;

	PushConstantRaster&                   pcRaster;
};

}