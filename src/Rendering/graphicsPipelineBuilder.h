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
	class GraphicsPipelineBuilder
	{
	public:
		GraphicsPipelineBuilder(Device& device, ResourceManager& resourceManager, RenderContext& renderContext, VkFormat offscreenColorFormat,
			PushConstantRaster& pcRaster);
		~GraphicsPipelineBuilder();


		void rebuild(VkExtent2D surfaceExtent);


		void draw(CommandBuffer& cmd);
		void update(CommandBuffer& cmd, Camera& camera, VkExtent2D surfaceExtent, std::vector<Light>& lights);

		void updateUniformBuffer(CommandBuffer& cmd, Camera& camera, VkExtent2D surfaceExtent);

		void createLightUniformsBuffer();
		void updateLightUniformsBuffer(CommandBuffer& cmd, const std::vector<Light>& shadowLights);

		void updateDescriptorSet(RenderTarget& dirShadowRenderTarget, RenderTarget& pointShadowRenderTarget,RenderTarget& offscreenRenderTarget);

		Buffer& getLightUniformsBuffer() { return *lightUniformsBuffer; }

		std::shared_ptr<DescriptorSetLayout> getDescriptorSetLayout() const { return descSetLayout; }
		std::unique_ptr<DescriptorPool>& getDescriptorPool() { return descPool; }
		const VkDescriptorSet& getDescriptorSet()       const { return descSet; }
		//GraphicsPipeline&                    getRasterPipeline()      const { return *rasterPipeline; }
		RenderPass& getRasterRenderPass() { return *rasterRenderPass; }
		GeometryRenderPass& getForwardRenderPass() { return *geomRenderPass; }

	private:



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

		DescriptorSetBindings				  gBufferDescSetBindings;
		std::shared_ptr<DescriptorSetLayout>  gBufferDescSetLayout;
		std::unique_ptr<DescriptorPool>       gBufferDescPool;
		VkDescriptorSet                       gBufferDescSet;


		std::unique_ptr<Buffer>               globalsBuffer;
		std::unique_ptr<Buffer>               objDescBuffer;
		std::unique_ptr<Buffer>               lightUniformsBuffer;

		std::unique_ptr<PipelineLayout>       rasterPipelineLayout;
		std::unique_ptr<RenderPass>           rasterRenderPass;
		//std::unique_ptr<GraphicsPipeline>      rasterPipeline;

		std::unique_ptr<GeometryRenderPass>    geomRenderPass;
		std::unique_ptr<LightingRenderPass>    lightingRenderPass;
		std::unique_ptr<SkyLightRenderPass>   skyLightRenderPass;


		Device& device;
		ResourceManager& resourceManager;
		RenderContext& renderContext;

		PushConstantRaster& pcRaster;


	};
}