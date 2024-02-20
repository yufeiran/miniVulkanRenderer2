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
	class GraphicsPipelineBuilder
	{
	public:
		GraphicsPipelineBuilder(Device& device, ResourceManager& resourceManager, RenderContext& renderContext, VkFormat offscreenColorFormat,
			PushConstantRaster& pcRaster);
		~GraphicsPipelineBuilder();


		void draw(CommandBuffer& cmd);
		void update(CommandBuffer& cmd,Camera& camera, VkExtent2D surfaceExtent);
		
		void updateUniformBuffer(CommandBuffer& cmd, Camera& camera, VkExtent2D surfaceExtent);



		std::shared_ptr<DescriptorSetLayout> getDescriptorSetLayout() const { return descSetLayout; }
		std::unique_ptr<DescriptorPool>&     getDescriptorPool()            { return descPool; }
		const VkDescriptorSet&               getDescriptorSet()       const { return descSet; }
		//GraphicsPipeline&                    getRasterPipeline()      const { return *rasterPipeline; }
		RenderPass&                          getRasterRenderPass()          { return *rasterRenderPass; }
		ForwardRenderPass&                   getForwardRenderPass()         { return *forwardRenderPass; }

	private:

		void updateDescriptorSet();

		void createDescriptorSetLayout();
		//void createRasterPipeline();
		void createUniformBuffer();
		void createObjDescriptionBuffer();
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
		//std::unique_ptr<GraphicsPipeline>      rasterPipeline;

		std::unique_ptr<ForwardRenderPass>    forwardRenderPass;


		Device &device;
		ResourceManager& resourceManager;
		RenderContext& renderContext;

		PushConstantRaster& pcRaster;

		
	};
}