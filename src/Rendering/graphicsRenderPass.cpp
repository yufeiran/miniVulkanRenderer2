#include "graphicsRenderPass.h"
#include "ResourceManagement/ResourceManager.h"
#include "Vulkan/commandBuffer.h"
#include "Vulkan/graphicsPipeline.h"

using namespace mini;
GraphicsRenderPass::GraphicsRenderPass
(Device& device, ResourceManager& resourceManager, VkExtent2D extent)
	:device(device), resourceManager(resourceManager), extent(extent)
{

}

GraphicsRenderPass::~GraphicsRenderPass()
{
}

GeometryRenderPass::GeometryRenderPass(Device& device,
	ResourceManager& resourceManager,
	const RenderPass& renderPass,
	VkExtent2D                            extent,
	std::shared_ptr<DescriptorSetLayout>  descSetLayout,
	PushConstantRaster& pcRaster,
	int                                   subpassIndex
)
	:GraphicsRenderPass(device, resourceManager, extent), renderPass(renderPass), pcRaster(pcRaster)
{
	rasterShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/geometry.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	rasterShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/geometry.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));

	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant = {};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);


	std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(descSetLayout);

	rasterPipelineLayout = std::make_unique<PipelineLayout>(device, descSetLayouts, pushConstants);


	rebuild(extent, subpassIndex);
}

mini::GeometryRenderPass::~GeometryRenderPass()
{
}

void GeometryRenderPass::update()
{

}

void GeometryRenderPass::draw(CommandBuffer& cmd, std::vector<VkDescriptorSet> descSet)
{
	VkDeviceSize offset{ 0 };

	cmd.setViewPortAndScissor(extent);

	auto& rasterPipeline = graphicsPipeline;

	cmd.bindPipeline(*rasterPipeline);


	cmd.bindDescriptorSet(descSet);

	resourceManager.draw(cmd, pcRaster);

}

void GeometryRenderPass::rebuild(VkExtent2D surfaceExtent, int subpassIndex)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(rasterShaderModules, *rasterPipelineLayout, device, surfaceExtent);

	auto colorBlendAttachment = graphicsPipeline->colorBlendAttachment;
	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;

	colorBlendAttachments.push_back(colorBlendAttachment); // position
	colorBlendAttachments.push_back(colorBlendAttachment); // normal
	colorBlendAttachments.push_back(colorBlendAttachment); // albedo
	colorBlendAttachments.push_back(colorBlendAttachment); // metallicRoughness
	colorBlendAttachments.push_back(colorBlendAttachment); // emissive
	colorBlendAttachments.push_back(colorBlendAttachment); // positionInView
	colorBlendAttachments.push_back(colorBlendAttachment); // normalInView

	graphicsPipeline->colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
	graphicsPipeline->colorBlending.pAttachments = colorBlendAttachments.data();


	graphicsPipeline->setSubpassIndex(subpassIndex);
	extent = surfaceExtent;
	graphicsPipeline->build(renderPass);
}

SkyLightRenderPass::SkyLightRenderPass(Device& device,
	ResourceManager& resourceManager,
	const RenderPass& renderPass,
	VkExtent2D                           extent,
	std::shared_ptr<DescriptorSetLayout> descSetLayout,
	PushConstantRaster& pcRaster,
	int                                  subpassIndex)
	:GraphicsRenderPass(device, resourceManager, extent), renderPass(renderPass), pcRaster(pcRaster), skyLightCube(device)
{
	skyLightShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/skybox.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	skyLightShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/skybox.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));

	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant = {};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);



	std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(descSetLayout);

	skyLightPipelineLayout = std::make_unique<PipelineLayout>(device, descSetLayouts, pushConstants);


	graphicsPipeline = std::make_unique<GraphicsPipeline>(skyLightShaderModules, *skyLightPipelineLayout, device, extent);
	graphicsPipeline->setSubpassIndex(subpassIndex);
	graphicsPipeline->depthStencil.depthTestEnable = VK_FALSE;
	graphicsPipeline->depthStencil.depthWriteEnable = VK_FALSE;
	graphicsPipeline->rasterizer.cullMode = VK_CULL_MODE_NONE;


	graphicsPipeline->build(renderPass);
}

SkyLightRenderPass::~SkyLightRenderPass()
{
}

void SkyLightRenderPass::update()
{

}

void SkyLightRenderPass::draw(CommandBuffer& cmd, std::vector<VkDescriptorSet> descSet)
{
	VkDeviceSize offset{ 0 };

	cmd.setViewPortAndScissor(extent);

	auto& rasterPipeline = graphicsPipeline;

	cmd.bindPipeline(*rasterPipeline);


	cmd.bindDescriptorSet(descSet);

	cmd.pushConstant(pcRaster, static_cast<VkShaderStageFlagBits>(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT));


	cmd.bindVertexBuffer(skyLightCube.getVertexBuffer());
	//cmd.bindIndexBuffer(*model->indexBuffer);

	cmd.draw(skyLightCube.getVertexCount(), 1, 0, 0);


}

void SkyLightRenderPass::rebuild(VkExtent2D surfaceExtent, int subpassIndex)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(skyLightShaderModules, *skyLightPipelineLayout, device, surfaceExtent);
	graphicsPipeline->setSubpassIndex(subpassIndex);
	graphicsPipeline->depthStencil.depthTestEnable = VK_FALSE;
	graphicsPipeline->depthStencil.depthWriteEnable = VK_FALSE;
	graphicsPipeline->rasterizer.cullMode = VK_CULL_MODE_NONE;
	extent = surfaceExtent;
	graphicsPipeline->build(renderPass);
}

DirShadowMapRenderPass::DirShadowMapRenderPass(Device& device,
	ResourceManager& resourceManager,
	const RenderPass& renderPass,
	VkExtent2D extent,
	std::shared_ptr<DescriptorSetLayout> descSetLayout,
	PushConstantRaster& pcRaster,
	int subpassIndex
) :GraphicsRenderPass(device, resourceManager, extent), renderPass(renderPass), pcRaster(pcRaster)
{
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/shadowMap.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/shadowMap.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));

	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant = {};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);



	std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(descSetLayout);

	pipelineLayout = std::make_unique<PipelineLayout>(device, descSetLayouts, pushConstants);


	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules, *pipelineLayout, device, extent);
	graphicsPipeline->setSubpassIndex(subpassIndex);


	graphicsPipeline->build(renderPass);
}

DirShadowMapRenderPass::~DirShadowMapRenderPass()
{
}

void DirShadowMapRenderPass::update()
{
}

void DirShadowMapRenderPass::draw(CommandBuffer& cmd, std::vector<VkDescriptorSet> descSet)
{
	VkDeviceSize offset{ 0 };

	cmd.setViewPortAndScissor(extent);

	auto& rasterPipeline = graphicsPipeline;

	cmd.bindPipeline(*rasterPipeline);


	cmd.bindDescriptorSet(descSet);

	resourceManager.draw(cmd, pcRaster);
}

void DirShadowMapRenderPass::rebuild(VkExtent2D surfaceExtent, int subpassIndex)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules, *pipelineLayout, device, extent);
	graphicsPipeline->setSubpassIndex(subpassIndex);


	graphicsPipeline->build(renderPass);
}

PointShadowMapRenderPass::PointShadowMapRenderPass(Device& device, ResourceManager& resourceManager, const RenderPass& renderPass, VkExtent2D extent, std::shared_ptr<DescriptorSetLayout> descSetLayout, PushConstantRaster& pcRaster, int subpassIndex)
	:GraphicsRenderPass(device, resourceManager, extent), renderPass(renderPass), pcRaster(pcRaster)
{
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/shadowMapPoint.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/shadowMapPoint.geom.spv", device, VK_SHADER_STAGE_GEOMETRY_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/shadowMapPoint.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));

	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant = {};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);



	std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(descSetLayout);

	pipelineLayout = std::make_unique<PipelineLayout>(device, descSetLayouts, pushConstants);


	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules, *pipelineLayout, device, extent);
	graphicsPipeline->setSubpassIndex(subpassIndex);


	graphicsPipeline->build(renderPass);

}

PointShadowMapRenderPass::~PointShadowMapRenderPass()
{

}

void PointShadowMapRenderPass::update()
{

}

void PointShadowMapRenderPass::draw(CommandBuffer& cmd, std::vector<VkDescriptorSet> descSet)
{
	VkDeviceSize offset{ 0 };

	cmd.setViewPortAndScissor(extent);

	auto& rasterPipeline = graphicsPipeline;

	cmd.bindPipeline(*rasterPipeline);


	cmd.bindDescriptorSet(descSet);

	resourceManager.draw(cmd, pcRaster);
}

void PointShadowMapRenderPass::rebuild(VkExtent2D surfaceExtent, int subpassIndex)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules, *pipelineLayout, device, extent);
	graphicsPipeline->setSubpassIndex(subpassIndex);


	graphicsPipeline->build(renderPass);
}

LightingRenderPass::LightingRenderPass(Device& device, ResourceManager& resourceManager, const RenderPass& renderPass, VkExtent2D extent, std::shared_ptr<DescriptorSetLayout> globalDescSetLayout, std::shared_ptr<DescriptorSetLayout> gBufferDescSetLayout, PushConstantRaster& pcRaster, int subpassIndex)
	:GraphicsRenderPass(device, resourceManager, extent), renderPass(renderPass), pcRaster(pcRaster), postQuad(device)
{
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/lighting.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/lighting.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));

	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant = {};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);


	std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(globalDescSetLayout);
	descSetLayouts.push_back(gBufferDescSetLayout);

	pipelineLayout = std::make_unique<PipelineLayout>(device, descSetLayouts, pushConstants);


	rebuild(extent, subpassIndex);
}

LightingRenderPass::~LightingRenderPass()
{

}

void LightingRenderPass::update()
{

}

void LightingRenderPass::draw(CommandBuffer& cmd, std::vector<VkDescriptorSet> descSet)
{

	cmd.bindPipeline(*graphicsPipeline);

	cmd.bindDescriptorSet(descSet);


	vkCmdPushConstants(cmd.getHandle(), pipelineLayout->getHandle(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantRaster), &pcRaster);
	cmd.bindVertexBuffer(postQuad.getVertexBuffer());

	cmd.setViewPortAndScissor(extent);

	cmd.draw(3, 1, 0, 0);
	cmd.draw(3, 1, 1, 0);


}

void LightingRenderPass::rebuild(VkExtent2D surfaceExtent, int subpassIndex)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules, *pipelineLayout, device, surfaceExtent);
	graphicsPipeline->rasterizer.cullMode = VK_CULL_MODE_NONE;
	graphicsPipeline->setSubpassIndex(subpassIndex);
	extent = surfaceExtent;
	graphicsPipeline->build(renderPass);
}

SSAORenderPass::SSAORenderPass(Device& device,
	ResourceManager& resourceManager,
	const RenderPass& renderPass,
	VkExtent2D                            extent,
	std::shared_ptr<DescriptorSetLayout>  descSetLayout,
	std::shared_ptr<DescriptorSetLayout>  ssaoDescSetLayout,
	PushConstantRaster& pcRaster,
	int                                   subpassIndex)
	:GraphicsRenderPass(device, resourceManager, extent), renderPass(renderPass), pcRaster(pcRaster), postQuad(device)
{
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/ssao.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/ssao.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));

	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant = {};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);


	std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(descSetLayout);
	descSetLayouts.push_back(ssaoDescSetLayout);

	pipelineLayout = std::make_unique<PipelineLayout>(device, descSetLayouts, pushConstants);


	rebuild(extent, subpassIndex);
}

SSAORenderPass::~SSAORenderPass()
{

}

void SSAORenderPass::update()
{

}

void SSAORenderPass::draw(CommandBuffer& cmd, const std::vector<VkDescriptorSet> descSet)
{
	cmd.bindPipeline(*graphicsPipeline);

	cmd.bindDescriptorSet(descSet);


	vkCmdPushConstants(cmd.getHandle(), pipelineLayout->getHandle(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantPost), &pcRaster);
	cmd.bindVertexBuffer(postQuad.getVertexBuffer());

	cmd.setViewPortAndScissor(extent);

	cmd.draw(3, 1, 0, 0);
	cmd.draw(3, 1, 1, 0);
}

void SSAORenderPass::rebuild(VkExtent2D surfaceExtent, int subpassIndex)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules, *pipelineLayout, device, surfaceExtent);
	graphicsPipeline->setSubpassIndex(subpassIndex);
	graphicsPipeline->rasterizer.cullMode = VK_CULL_MODE_NONE;
	extent = surfaceExtent;
	graphicsPipeline->build(renderPass);
}




SSAOBlurRenderPass::SSAOBlurRenderPass(Device& device,
	ResourceManager& resourceManager,
	const RenderPass& renderPass,
	VkExtent2D extent,
	std::shared_ptr<DescriptorSetLayout> ssaoBlurDescSetLayout,
	PushConstantRaster& pcRaster, int subpassIndex)
	:GraphicsRenderPass(device, resourceManager, extent), renderPass(renderPass), pcRaster(pcRaster), postQuad(device)
{
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/ssao.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/ssaoBlur.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));


	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant = {};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);


	std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(ssaoBlurDescSetLayout);

	pipelineLayout = std::make_unique<PipelineLayout>(device, descSetLayouts, pushConstants);


	rebuild(extent, subpassIndex);

}

mini::SSAOBlurRenderPass::~SSAOBlurRenderPass()
{

}

void mini::SSAOBlurRenderPass::update()
{
}

void SSAOBlurRenderPass::draw(CommandBuffer& cmd, const std::vector<VkDescriptorSet> descSet)
{
	cmd.bindPipeline(*graphicsPipeline);

	cmd.bindDescriptorSet(descSet);


	vkCmdPushConstants(cmd.getHandle(), pipelineLayout->getHandle(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantRaster), &pcRaster);
	cmd.bindVertexBuffer(postQuad.getVertexBuffer());

	cmd.setViewPortAndScissor(extent);

	cmd.draw(3, 1, 0, 0);
	cmd.draw(3, 1, 1, 0);
}

void SSAOBlurRenderPass::rebuild(VkExtent2D surfaceExtent, int subpassIndex)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules, *pipelineLayout, device, surfaceExtent);
	graphicsPipeline->setSubpassIndex(subpassIndex);
	graphicsPipeline->rasterizer.cullMode = VK_CULL_MODE_NONE;
	extent = surfaceExtent;
	graphicsPipeline->build(renderPass);
}

PBBDownSamplingRenderPass::PBBDownSamplingRenderPass(Device& device,
	ResourceManager& resourceManager,
	const RenderPass& renderPass,
	VkExtent2D extent,
	std::shared_ptr<DescriptorSetLayout> pbbDescSetLayout,
	PushConstantPost& pcPost,
	int subpassIndex)
	:GraphicsRenderPass(device, resourceManager, extent), renderPass(renderPass), pcPost(pcPost), postQuad(device)
{
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/post.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/pbbloomDownSampling.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));

	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant = {};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantPost);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);

	std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(pbbDescSetLayout);

	pipelineLayout = std::make_unique<PipelineLayout>(device, descSetLayouts, pushConstants);

	rebuild(extent, subpassIndex);

}

mini::PBBDownSamplingRenderPass::~PBBDownSamplingRenderPass()
{

}

void mini::PBBDownSamplingRenderPass::update()
{
}

void mini::PBBDownSamplingRenderPass::draw(CommandBuffer& cmd, const std::vector<VkDescriptorSet> descSet)
{
	cmd.bindPipeline(*graphicsPipeline);

	cmd.bindDescriptorSet(descSet);


	vkCmdPushConstants(cmd.getHandle(), pipelineLayout->getHandle(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantPost), &pcPost);
	cmd.bindVertexBuffer(postQuad.getVertexBuffer());

	cmd.setViewPortAndScissor(extent);

	cmd.draw(3, 1, 0, 0);
	cmd.draw(3, 1, 1, 0);
}

void mini::PBBDownSamplingRenderPass::rebuild(VkExtent2D surfaceExtent, int subpassIndex)
{

	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules, *pipelineLayout, device, extent);

	graphicsPipeline->rasterizer.cullMode = VK_CULL_MODE_NONE;
	graphicsPipeline->setSubpassIndex(subpassIndex);

	graphicsPipeline->build(renderPass);
}

mini::PBBUpSamplingRenderPass::PBBUpSamplingRenderPass(
	Device& device,
	ResourceManager& resourceManager,
	const RenderPass& renderPass,
	VkExtent2D extent,
	std::shared_ptr<DescriptorSetLayout> pbbDescSetLayout,
	PushConstantPost& pcPost,
	int subpassIndex)
	:GraphicsRenderPass(device, resourceManager, extent), renderPass(renderPass), pcPost(pcPost), postQuad(device)
{
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/post.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/pbbloomUpSampling.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));

	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant = {};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantPost);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);

	std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(pbbDescSetLayout);

	pipelineLayout = std::make_unique<PipelineLayout>(device, descSetLayouts, pushConstants);

	rebuild(extent, subpassIndex);
}

mini::PBBUpSamplingRenderPass::~PBBUpSamplingRenderPass()
{
}

void mini::PBBUpSamplingRenderPass::update()
{
}

void mini::PBBUpSamplingRenderPass::draw(CommandBuffer& cmd, const std::vector<VkDescriptorSet> descSet)
{
	cmd.bindPipeline(*graphicsPipeline);

	cmd.bindDescriptorSet(descSet);


	vkCmdPushConstants(cmd.getHandle(), pipelineLayout->getHandle(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantPost), &pcPost);
	cmd.bindVertexBuffer(postQuad.getVertexBuffer());

	cmd.setViewPortAndScissor(extent);

	cmd.draw(3, 1, 0, 0);
	cmd.draw(3, 1, 1, 0);
}

void mini::PBBUpSamplingRenderPass::rebuild(VkExtent2D surfaceExtent, int subpassIndex)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules, *pipelineLayout, device, extent);

	graphicsPipeline->setSubpassIndex(subpassIndex);

	graphicsPipeline->rasterizer.cullMode = VK_CULL_MODE_NONE;


	graphicsPipeline->build(renderPass);
}

mini::SSRRenderPass::SSRRenderPass(Device& device, ResourceManager& resourceManager, const RenderPass& renderPass, VkExtent2D extent, std::shared_ptr<DescriptorSetLayout> descSetLayout, std::shared_ptr<DescriptorSetLayout> ssrDescSetLayout, PushConstantRaster& pcRaster, int subpassIndex)
	:GraphicsRenderPass(device, resourceManager, extent), renderPass(renderPass), pcRaster(pcRaster), postQuad(device)
{
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/ssr.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/ssr.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));

	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant = {};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);

	std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(descSetLayout);
	descSetLayouts.push_back(ssrDescSetLayout);

	pipelineLayout = std::make_unique<PipelineLayout>(device, descSetLayouts, pushConstants);

	rebuild(extent, subpassIndex);
}

mini::SSRRenderPass::~SSRRenderPass()
{

}

void mini::SSRRenderPass::update()
{

}

void mini::SSRRenderPass::draw(CommandBuffer& cmd, const std::vector<VkDescriptorSet> descSet)
{
	cmd.bindPipeline(*graphicsPipeline);

	cmd.bindDescriptorSet(descSet);


	vkCmdPushConstants(cmd.getHandle(), pipelineLayout->getHandle(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantPost), &pcRaster);
	cmd.bindVertexBuffer(postQuad.getVertexBuffer());

	cmd.setViewPortAndScissor(extent);

	cmd.draw(3, 1, 0, 0);
	cmd.draw(3, 1, 1, 0);
}

void mini::SSRRenderPass::rebuild(VkExtent2D surfaceExtent, int subpassIndex)
{
	this->extent = surfaceExtent;

	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules, *pipelineLayout, device, extent);

	graphicsPipeline->setSubpassIndex(subpassIndex);

	graphicsPipeline->rasterizer.cullMode = VK_CULL_MODE_NONE;

	graphicsPipeline->build(renderPass);
}

mini::SSRBlurRenderPass::SSRBlurRenderPass(Device& device, ResourceManager& resourceManager, const RenderPass& renderPass, VkExtent2D extent, std::shared_ptr<DescriptorSetLayout> ssaoBlurDescSetLayout, PushConstantRaster& pcRaster, int subpassIndex)
:GraphicsRenderPass(device, resourceManager, extent), renderPass(renderPass), pcRaster(pcRaster), postQuad(device)
{
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/ssao.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/ssrBlur.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));


	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant = {};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);


	std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(ssaoBlurDescSetLayout);

	pipelineLayout = std::make_unique<PipelineLayout>(device, descSetLayouts, pushConstants);


	rebuild(extent, subpassIndex);

}
mini::SSRBlurRenderPass::~SSRBlurRenderPass()
{
}

void mini::SSRBlurRenderPass::update()
{
}

void mini::SSRBlurRenderPass::draw(CommandBuffer& cmd, const std::vector<VkDescriptorSet> descSet)
{
	cmd.bindPipeline(*graphicsPipeline);

	cmd.bindDescriptorSet(descSet);


	vkCmdPushConstants(cmd.getHandle(), pipelineLayout->getHandle(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantRaster), &pcRaster);
	cmd.bindVertexBuffer(postQuad.getVertexBuffer());

	cmd.setViewPortAndScissor(extent);

	cmd.draw(3, 1, 0, 0);
	cmd.draw(3, 1, 1, 0);
}

void mini::SSRBlurRenderPass::rebuild(VkExtent2D surfaceExtent, int subpassIndex)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules, *pipelineLayout, device, surfaceExtent);
	graphicsPipeline->setSubpassIndex(subpassIndex);
	graphicsPipeline->rasterizer.cullMode = VK_CULL_MODE_NONE;
	extent = surfaceExtent;
	graphicsPipeline->build(renderPass);
}
