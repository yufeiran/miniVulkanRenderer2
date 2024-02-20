#pragma once

#include "Common/common.h"
#include "Vulkan/descriptorSetLayout.h"

#include "Vulkan/shaderModule.h"
#include "Vulkan/renderPass.h"
#include "Vulkan/graphicsPipeline.h"
#include "ResourceManagement/ResourceManager.h"

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
		GraphicsRenderPass(Device&          device, 
			               ResourceManager& resourceManager, 
			               VkExtent2D       extent);
		~GraphicsRenderPass();

		virtual void update() = 0;
		virtual void draw(CommandBuffer& cmd,VkDescriptorSet descSet) = 0;

		GraphicsPipeline& getGraphicsPipeline() { return *graphicsPipeline; }
	protected:
		Device& device;
		ResourceManager& resourceManager;
		VkExtent2D  extent;

		std::unique_ptr<GraphicsPipeline> graphicsPipeline;


	};

	class ForwardRenderPass :public GraphicsRenderPass
	{
	public:
		ForwardRenderPass(Device& device, 
						  ResourceManager& resourceManager,
						  const RenderPass& renderPass,
						  VkExtent2D extent,
			              std::shared_ptr<DescriptorSetLayout> descSetLayout,
						  PushConstantRaster& pcRaster
		);
		~ForwardRenderPass();

		void update();
		void draw(CommandBuffer& cmd,VkDescriptorSet descSet);
	private:
		const RenderPass& renderPass;

		PushConstantRaster& pcRaster;

		std::vector<std::unique_ptr<ShaderModule>>rasterShaderModules;
		std::unique_ptr<PipelineLayout>       rasterPipelineLayout;

	};
}