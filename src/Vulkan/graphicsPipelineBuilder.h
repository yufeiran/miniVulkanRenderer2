#pragma once

#include "Common/common.h"
#include "Vulkan/shaderModule.h"
#include "Vulkan/descriptorSetBindings.h"
#include "Vulkan/descriptorSetLayout.h"
#include "Vulkan/pipelineLayout.h"
#include "Vulkan/renderPass.h"
#include "Vulkan/graphicPipeline.h"
#include "Rendering/renderContext.h"

namespace mini
{
	class GraphicsPipelineBuilder
	{
	public:
		GraphicsPipelineBuilder(Device& device, ResourceManager& resourceManager, RenderContext& renderContext, VkFormat offscreenColorFormat);
		~GraphicsPipelineBuilder();

		
		void createDescriptorSetLayout();
		void createRasterPipeline();
		void createUniformBuffer();
		void createObjDescriptionBuffer();
		void updateDescriptorSet();
		void updateUniformBuffer(CommandBuffer& cmd, Camera& camera, VkExtent2D surfaceExtent);

		std::shared_ptr<DescriptorSetLayout> getDescriptorSetLayout() const { return descSetLayout; }
		std::unique_ptr<DescriptorPool>& getDescriptorPool() { return descPool; }
		const VkDescriptorSet& getDescriptorSet() const { return descSet; }
		GraphicPipeline& getRasterPipeline() const { return *rasterPipeline; }
		RenderPass& getRasterRenderPass()  { return *rasterRenderPass; }

	private:
		VkFormat offscreenColorFormat;
		VkExtent2D surfaceExtent{};
		// raster pipeline data
		std::vector<std::unique_ptr<ShaderModule>> rasterShaderModules;

		DescriptorSetBindings                 descSetBindings;
		std::shared_ptr<DescriptorSetLayout>  descSetLayout;
		std::unique_ptr<DescriptorPool>       descPool;
		VkDescriptorSet                       descSet;
		std::unique_ptr<Buffer>               globalsBuffer;
		std::unique_ptr<Buffer>               objDescBuffer;

		std::unique_ptr<PipelineLayout>       rasterPipelineLayout;
		std::unique_ptr<RenderPass>           rasterRenderPass;
		std::unique_ptr<GraphicPipeline>      rasterPipeline;


		Device &device;
		ResourceManager& resourceManager;
		RenderContext& renderContext;
	};
}