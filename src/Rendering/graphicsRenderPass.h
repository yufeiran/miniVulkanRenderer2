#pragma once

#include "Common/common.h"
#include "Vulkan/descriptorSetLayout.h"

#include "Vulkan/shaderModule.h"
#include "Vulkan/renderPass.h"
#include "Vulkan/graphicsPipeline.h"
#include "ResourceManagement/ResourceManager.h"
#include "Rendering/skyLightCube.h"
#include "postQuad.h"

namespace mini
{
	class Device;
	class CommandBuffer;
	class ResourceManager;
	class GraphicsPipeline;
	class ShaderModule;

	class GraphicsRenderPass
	{
	public:
		GraphicsRenderPass(Device& device,
			ResourceManager& resourceManager,
			VkExtent2D       extent);
		~GraphicsRenderPass();

		virtual void update() = 0;
		virtual void draw(CommandBuffer& cmd, std::vector<VkDescriptorSet> descSet) = 0;

		GraphicsPipeline& getGraphicsPipeline() { return *graphicsPipeline; }
	protected:
		Device& device;
		ResourceManager& resourceManager;
		VkExtent2D  extent;

		std::unique_ptr<GraphicsPipeline> graphicsPipeline;


	};

	class SkyLightRenderPass :public GraphicsRenderPass
	{
	public:
		SkyLightRenderPass(Device& device,
			ResourceManager& resourceManager,
			const RenderPass& renderPass,
			VkExtent2D extent,
			std::shared_ptr<DescriptorSetLayout> descSetLayout,
			PushConstantRaster& pcRaster,
			int subpassIndex = 0
		);
		~SkyLightRenderPass();

		void update();
		void draw(CommandBuffer& cmd, std::vector<VkDescriptorSet> descSet);

		void rebuild(VkExtent2D surfaceExtent, int subpassIndex = 0);

	private:
		const RenderPass& renderPass;

		PushConstantRaster& pcRaster;

		SkyLightCube skyLightCube;

		std::vector<std::unique_ptr<ShaderModule>>skyLightShaderModules;
		std::unique_ptr<PipelineLayout>       skyLightPipelineLayout;


	};

	class DirShadowMapRenderPass :public GraphicsRenderPass
	{
	public:
		DirShadowMapRenderPass(Device& device,
			ResourceManager& resourceManager,
			const RenderPass& renderPass,
			VkExtent2D extent,
			std::shared_ptr<DescriptorSetLayout> descSetLayout,
			PushConstantRaster& pcRaster,
			int subpassIndex = 1
		);
		~DirShadowMapRenderPass();

		void update();
		void draw(CommandBuffer& cmd, std::vector<VkDescriptorSet> descSet);

		void rebuild(VkExtent2D surfaceExtent, int subpassIndex = 1);
	private:
		const RenderPass& renderPass;

		PushConstantRaster& pcRaster;

		std::vector<std::unique_ptr<ShaderModule>> shaderModules;
		std::unique_ptr<PipelineLayout>            pipelineLayout;
	};

	class PointShadowMapRenderPass :public GraphicsRenderPass
	{
	public:
		PointShadowMapRenderPass(Device& device,
			ResourceManager& resourceManager,
			const RenderPass& renderPass,
			VkExtent2D extent,
			std::shared_ptr<DescriptorSetLayout> descSetLayout,
			PushConstantRaster& pcRaster,
			int subpassIndex = 1
		);
		~PointShadowMapRenderPass();

		void update();
		void draw(CommandBuffer& cmd, std::vector<VkDescriptorSet> descSet);

		void rebuild(VkExtent2D surfaceExtent, int subpassIndex = 1);
	private:
		const RenderPass& renderPass;

		PushConstantRaster& pcRaster;

		std::vector<std::unique_ptr<ShaderModule>> shaderModules;
		std::unique_ptr<PipelineLayout>            pipelineLayout;

	};

	class GeometryRenderPass :public GraphicsRenderPass
	{
	public:
		GeometryRenderPass(Device& device,
			ResourceManager& resourceManager,
			const RenderPass& renderPass,
			VkExtent2D extent,
			std::shared_ptr<DescriptorSetLayout> descSetLayout,
			PushConstantRaster& pcRaster,
			int subpassIndex = 2
		);
		~GeometryRenderPass();

		void update();
		void draw(CommandBuffer& cmd, std::vector<VkDescriptorSet> descSet);

		void rebuild(VkExtent2D surfaceExtent, int subpassIndex = 2);
	private:
		const RenderPass& renderPass;

		PushConstantRaster& pcRaster;

		std::vector<std::unique_ptr<ShaderModule>>rasterShaderModules;
		std::unique_ptr<PipelineLayout>       rasterPipelineLayout;

	};

	class LightingRenderPass :public GraphicsRenderPass
	{
	public:
		LightingRenderPass(Device& device,
			ResourceManager& resourceManager,
			const RenderPass& renderPass,
			VkExtent2D extent,
			std::shared_ptr<DescriptorSetLayout> descSetLayout,
			std::shared_ptr<DescriptorSetLayout> gBufferDescSetLayout,
			PushConstantRaster& pcRaster,
			int subpassIndex = 2
		);
		~LightingRenderPass();

		void update();
		void draw(CommandBuffer& cmd, std::vector<VkDescriptorSet> descSet);

		void rebuild(VkExtent2D surfaceExtent, int subpassIndex = 2);
	private:
		PostQuad postQuad;
		const RenderPass& renderPass;

		PushConstantRaster& pcRaster;

		std::vector<std::unique_ptr<ShaderModule>> shaderModules;
		std::unique_ptr<PipelineLayout>            pipelineLayout;

	};

	class SSAORenderPass :public GraphicsRenderPass
	{
	public:
		SSAORenderPass(Device& device,
			ResourceManager& resourceManager,
			const RenderPass& renderPass,
			VkExtent2D extent,
			std::shared_ptr<DescriptorSetLayout> descSetLayout,
			std::shared_ptr<DescriptorSetLayout> ssaoDescSetLayout,
			PushConstantRaster& pcRaster,
			int subpassIndex = 3
		);
		~SSAORenderPass();

		void update();
		void draw(CommandBuffer& cmd,const std::vector<VkDescriptorSet> descSet);

		void rebuild(VkExtent2D surfaceExtent, int subpassIndex = 3);
	private:
		PostQuad postQuad;
		const RenderPass& renderPass;
		PushConstantRaster& pcRaster;
		std::vector<std::unique_ptr<ShaderModule>> shaderModules;
		std::unique_ptr<PipelineLayout>            pipelineLayout;

	};

	class SSAOBlurRenderPass :public GraphicsRenderPass
	{
	public:
		SSAOBlurRenderPass(Device& device,
			ResourceManager& resourceManager,
			const RenderPass& renderPass,
			VkExtent2D extent,
			std::shared_ptr<DescriptorSetLayout> ssaoBlurDescSetLayout,
			PushConstantRaster& pcRaster,
			int subpassIndex = 3
		);
		~SSAOBlurRenderPass();

		void update();
		void draw(CommandBuffer& cmd,const std::vector<VkDescriptorSet> descSet);

		void rebuild(VkExtent2D surfaceExtent, int subpassIndex = 3);
	private:
		PostQuad postQuad;
		const RenderPass& renderPass;
		PushConstantRaster& pcRaster;
		std::vector<std::unique_ptr<ShaderModule>> shaderModules;
		std::unique_ptr<PipelineLayout>            pipelineLayout;

	};




}