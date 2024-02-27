#include "graphicsRenderPass.h"
#include "ResourceManagement/ResourceManager.h"
#include "Vulkan/commandBuffer.h"
#include "Vulkan/graphicsPipeline.h"

using namespace mini;
GraphicsRenderPass::GraphicsRenderPass
(Device& device, ResourceManager& resourceManager,VkExtent2D extent)
	:device(device),resourceManager(resourceManager),extent(extent)
{

}

GraphicsRenderPass::~GraphicsRenderPass()
{
}

ForwardRenderPass::ForwardRenderPass(Device&                               device, 
									 ResourceManager&                      resourceManager,
									 const RenderPass&                     renderPass, 
									 VkExtent2D                            extent,
									 std::shared_ptr<DescriptorSetLayout>  descSetLayout,
									 PushConstantRaster&                   pcRaster,
									 int                                   subpassIndex
	)
	:GraphicsRenderPass(device,resourceManager,extent),renderPass(renderPass),pcRaster(pcRaster)
{
	rasterShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/vertexShader.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	rasterShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/fragmentShader.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));
	
	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant={};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);

	//VkDescriptorSetLayoutBinding uboLayoutBinding{};
	//uboLayoutBinding.binding = 0;
	//uboLayoutBinding.descriptorCount = 1;
	//uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	//uboLayoutBinding.pImmutableSamplers = nullptr;

	//VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	//samplerLayoutBinding.binding = 1;
	//samplerLayoutBinding.descriptorCount=1;
	//samplerLayoutBinding.descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	//samplerLayoutBinding.pImmutableSamplers = nullptr;

	//std::vector<VkDescriptorSetLayoutBinding>layoutBindings{uboLayoutBinding,samplerLayoutBinding};



	//rasterDescriptorSetLayouts.push_back(std::make_unique<DescriptorSetLayout>(*device,layoutBindings));

    std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(descSetLayout);

	rasterPipelineLayout = std::make_unique<PipelineLayout>(device,descSetLayouts,pushConstants);


	graphicsPipeline = std::make_unique<GraphicsPipeline>(rasterShaderModules,*rasterPipelineLayout,device,extent);

	graphicsPipeline->setSubpassIndex(subpassIndex);

	graphicsPipeline->build(renderPass);
}

mini::ForwardRenderPass::~ForwardRenderPass()
{
}

void ForwardRenderPass::update()
{

}

void ForwardRenderPass::draw(CommandBuffer& cmd, VkDescriptorSet descSet)
{
	VkDeviceSize offset{0};

	cmd.setViewPortAndScissor(extent);

	auto& rasterPipeline = graphicsPipeline;

	cmd.bindPipeline(*rasterPipeline);

	
	cmd.bindDescriptorSet(descSet);

	for(const ObjInstance& inst:resourceManager.instances)
	{
		auto& model          = resourceManager.objModel[inst.objIndex];
		pcRaster.objIndex    = inst.objIndex;
		pcRaster.modelMatrix = inst.transform;
		cmd.pushConstant(pcRaster,static_cast<VkShaderStageFlagBits>( VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT));
		
		cmd.bindVertexBuffer(*model->vertexBuffer);
		cmd.bindIndexBuffer(*model->indexBuffer);
		vkCmdDrawIndexed(cmd.getHandle(),model->nbIndices,1,0,0,0);

	}


}

void ForwardRenderPass::rebuild(VkExtent2D surfaceExtent,int subpassIndex)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(rasterShaderModules,*rasterPipelineLayout,device,surfaceExtent);
	graphicsPipeline->setSubpassIndex(subpassIndex);
	extent = surfaceExtent;
	graphicsPipeline->build(renderPass);
}

SkyLightRenderPass::SkyLightRenderPass(Device&                               device,
										ResourceManager&                     resourceManager, 
										const RenderPass&                    renderPass, 
										VkExtent2D                           extent, 
										std::shared_ptr<DescriptorSetLayout> descSetLayout, 
										PushConstantRaster&                  pcRaster,
										int                                  subpassIndex)
	:GraphicsRenderPass(device,resourceManager,extent),renderPass(renderPass),pcRaster(pcRaster),skyLightCube(device)
{
	skyLightShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/skybox.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	skyLightShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/skybox.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));
	
	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant={};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);



    std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(descSetLayout);

	skyLightPipelineLayout = std::make_unique<PipelineLayout>(device,descSetLayouts,pushConstants);


	graphicsPipeline = std::make_unique<GraphicsPipeline>(skyLightShaderModules,*skyLightPipelineLayout,device,extent);
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

void SkyLightRenderPass::draw(CommandBuffer& cmd, VkDescriptorSet descSet)
{
	VkDeviceSize offset{0};

	cmd.setViewPortAndScissor(extent);

	auto& rasterPipeline = graphicsPipeline;

	cmd.bindPipeline(*rasterPipeline);

	
	cmd.bindDescriptorSet(descSet);

	cmd.pushConstant(pcRaster,static_cast<VkShaderStageFlagBits>( VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT));
		
		
	cmd.bindVertexBuffer(skyLightCube.getVertexBuffer());
	//cmd.bindIndexBuffer(*model->indexBuffer);

	cmd.draw(skyLightCube.getVertexCount(),1,0,0);


}

void SkyLightRenderPass::rebuild(VkExtent2D surfaceExtent,int subpassIndex)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(skyLightShaderModules,*skyLightPipelineLayout,device,surfaceExtent);
	graphicsPipeline->setSubpassIndex(subpassIndex);
	graphicsPipeline->depthStencil.depthTestEnable = VK_FALSE;
	graphicsPipeline->depthStencil.depthWriteEnable = VK_FALSE;
	graphicsPipeline->rasterizer.cullMode = VK_CULL_MODE_NONE;
	extent = surfaceExtent;
	graphicsPipeline->build(renderPass);
}

ShadowMapRenderPass::ShadowMapRenderPass(Device& device, 
										 ResourceManager& resourceManager, 
										 const RenderPass& renderPass, 
										 VkExtent2D extent, 
										 std::shared_ptr<DescriptorSetLayout> descSetLayout, 
										 PushConstantRaster& pcRaster, 
										 int subpassIndex
):GraphicsRenderPass(device,resourceManager,extent),renderPass(renderPass),pcRaster(pcRaster)
{
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/shadowMap.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
	shaderModules.push_back(std::make_unique<ShaderModule>("../../spv/shadowMap.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));
	
	std::vector<VkPushConstantRange> pushConstants;
	VkPushConstantRange pushConstant={};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(PushConstantRaster);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.push_back(pushConstant);



    std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts;
	descSetLayouts.push_back(descSetLayout);

	pipelineLayout = std::make_unique<PipelineLayout>(device,descSetLayouts,pushConstants);


	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules,*pipelineLayout,device,extent);
	graphicsPipeline->setSubpassIndex(subpassIndex);


	graphicsPipeline->build(renderPass);
}

ShadowMapRenderPass::~ShadowMapRenderPass()
{
}

void ShadowMapRenderPass::update()
{
}

void ShadowMapRenderPass::draw(CommandBuffer& cmd, VkDescriptorSet descSet)
{
	VkDeviceSize offset{0};

	cmd.setViewPortAndScissor(extent);

	auto& rasterPipeline = graphicsPipeline;

	cmd.bindPipeline(*rasterPipeline);

	
	cmd.bindDescriptorSet(descSet);

	for(const ObjInstance& inst:resourceManager.instances)
	{
		auto& model          = resourceManager.objModel[inst.objIndex];
		pcRaster.objIndex    = inst.objIndex;
		pcRaster.modelMatrix = inst.transform;
		cmd.pushConstant(pcRaster,static_cast<VkShaderStageFlagBits>( VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT));
		
		cmd.bindVertexBuffer(*model->vertexBuffer);
		cmd.bindIndexBuffer(*model->indexBuffer);
		vkCmdDrawIndexed(cmd.getHandle(),model->nbIndices,1,0,0,0);

	}
}

void ShadowMapRenderPass::rebuild(VkExtent2D surfaceExtent, int subpassIndex)
{
	graphicsPipeline = std::make_unique<GraphicsPipeline>(shaderModules,*pipelineLayout,device,extent);
	graphicsPipeline->setSubpassIndex(subpassIndex);


	graphicsPipeline->build(renderPass);
}
