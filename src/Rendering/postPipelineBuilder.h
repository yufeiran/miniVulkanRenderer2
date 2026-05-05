#pragma once 

#include "Common/common.h"
#include "Vulkan/shaderModule.h"
#include "Vulkan/descriptorSetLayout.h"
#include "Vulkan/descriptorSetBindings.h"
#include "Vulkan/descriptorPool.h"
#include "Vulkan/pipelineLayout.h"
#include "Rendering/postQuad.h"
#include "Vulkan/renderPass.h"
#include "Vulkan/Sampler.h"
#include "Vulkan/Device.h"
#include "Vulkan/GraphicsPipeline.h"


namespace mini
{
	struct PostInputTextures {
		const ImageView& offscreenColor;
		const ImageView& shadowMap;
		const ImageView& bloomTexture;
		const ImageView& ssrTexture;
		const Sampler& defaultSampler;
	};

	class PostPipelineBuilder
	{
	public:
		void initPostRender(Device& device, RenderTarget& offscreenRenderTarget, VkFormat postSurfaceColorFormat, 
		VkExtent2D surfaceExtent, VkFormat swapChainFormat);

		void updatePostDescriptorSet(Device& device,const PostInputTextures &postInputTextures);

		void drawPostBegin(CommandBuffer& cmd, const std::vector<VkClearValue>& clearValues,
			FrameBuffer& framebuffer, PushConstantPost& pcPost);

		void drawPostEnd(CommandBuffer& cmd, const std::vector<VkClearValue>& clearValues,
			FrameBuffer& framebuffer, PushConstantPost& pcPost);

		std::vector<std::unique_ptr<ShaderModule>>         postShaderModules;
		std::vector<std::shared_ptr<DescriptorSetLayout>>  postDescriptorSetLayouts;
		DescriptorSetBindings                              postDescSetBind;
		std::unique_ptr<DescriptorPool>                    postDescriptorPool;
		VkDescriptorSet                                    postDescriptorSet = VK_NULL_HANDLE;
		std::unique_ptr<PipelineLayout>                    postPipelineLayout;
		std::unique_ptr<RenderPass>                        postRenderPass;
		std::unique_ptr<GraphicsPipeline>                  postPipeline;
		std::unique_ptr<PostQuad>                          postQuad;
		std::unique_ptr<Sampler>                           postRenderImageSampler;
	private:

	};


}