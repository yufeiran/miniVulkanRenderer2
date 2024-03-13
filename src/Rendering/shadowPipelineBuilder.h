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


class ShadowPipelineBuilder
{
public:
	ShadowPipelineBuilder(Device& device, ResourceManager& resourceManager,PushConstantRaster& pcRaster, Buffer&  lightUniformsBuffer);
	~ShadowPipelineBuilder();

	void createDescriptorSetLayout();
	void createRenderTarget();

	void updateDescriptorSet();

	RenderTarget& getDirRenderTarget() { return *dirShadowRenderTarget; }
	RenderTarget& getPointRenderTarget() { return *pointShadowRenderTarget; }

	void draw(CommandBuffer& cmd);




private:


	Device&                               device;		
	ResourceManager&                      resourceManager;
	DescriptorSetBindings                 descSetBindings;
	std::shared_ptr<DescriptorSetLayout>  descSetLayout;
	std::unique_ptr<DescriptorPool>       descPool;
	VkDescriptorSet                       descSet;


	std::unique_ptr<PipelineLayout>       pipelineLayout;
	std::unique_ptr<RenderPass>           dirShadowRenderPass;

	std::unique_ptr<RenderPass>           pointShadowRenderPass;

	std::unique_ptr<RenderTarget>         dirShadowRenderTarget;
	std::unique_ptr<FrameBuffer>          dirShadowFramebuffer;

	std::unique_ptr<RenderTarget>         pointShadowRenderTarget;
	std::unique_ptr<FrameBuffer>          pointShadowFramebuffer;

	std::unique_ptr<DirShadowMapRenderPass>  shadowMapRenderPass;

	std::unique_ptr<PointShadowMapRenderPass>  pointShadowMapRenderPass;

	PushConstantRaster&                   pcRaster;

	Buffer&               lightUniformsBuffer;
};

}