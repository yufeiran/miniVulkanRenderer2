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
#include "Vulkan/buffer.h"

namespace mini
{

class Device;
class ResourceManager;
class DescriptorSetBindings;
class DescriptorSetLayout;
class DescriptorPool;


class PBBloomPipelineBuilder
{
public:
	PBBloomPipelineBuilder(Device& device, ResourceManager& resourceManager,
		VkExtent2D extent,
		RenderTarget                          &gBufferRenderTarget,
		VkFormat offscreenColorFormat,
		PushConstantPost& pcPost,
		int N);
	~PBBloomPipelineBuilder();

	void rebuild(VkExtent2D extent,RenderTarget& gBufferRenderTarget,int N);

	void createDescriptorSetLayout();
	

	void updateDescriptorSet();


	void draw(CommandBuffer& cmd);

	void createRenderTarget();

	void createBloomSizeBuffers();

	void calculateBloomSizes();

	std::vector<std::unique_ptr<RenderTarget>> & getRenderTargets() { return renderTargets; }

private:
	VkExtent2D							  extent;
	int                                   N;

	VkFormat                              offscreenColorFormat;

	Device&                               device;		
	ResourceManager&                      resourceManager;
	RenderTarget                          *gBufferRenderTarget;

	PushConstantPost&                     pcPost; 


	std::vector<std::unique_ptr<PipelineLayout>>       pipelineLayouts;
	std::vector<std::unique_ptr<RenderPass>>           downSamplingRenderPasses;
	std::vector<std::unique_ptr<RenderPass>>           upSamplingRenderPasses;

	std::vector<std::unique_ptr<PBBDownSamplingRenderPass>>       pbbDownSamplingRenderPasses;
	std::vector<std::unique_ptr<PBBUpSamplingRenderPass>>         pbbUpSamplingRenderPasses;


	std::vector<std::unique_ptr<FrameBuffer>>          downSamplingFramebuffers;
	std::vector<std::unique_ptr<FrameBuffer>>          upSamplingFramebuffers;

	std::vector<std::unique_ptr<RenderTarget>>         renderTargets;



	DescriptorSetBindings                              descSetBindings;
	std::shared_ptr<DescriptorSetLayout>               descSetLayout;
	std::unique_ptr<DescriptorPool>                    descPool;
	std::vector<VkDescriptorSet>                       downSamplingDescSets;
	std::vector<VkDescriptorSet>                       upSamplingDescSets;
	
	std::vector<glm::vec2>							   bloomInputTextureSizes;

	std::vector<std::unique_ptr<Buffer>>               bloomSizeBuffers;


};

}