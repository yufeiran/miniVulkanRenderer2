#include "Vulkan/graphicsPipelineBuilder.h"
#include "ResourceManagement/ResourceManager.h"
#include "Common/camera.h"
#include "Vulkan/commandBuffer.h"
#include "Vulkan/buffer.h"


using namespace mini;

GraphicsPipelineBuilder::GraphicsPipelineBuilder(Device& device, ResourceManager& resourceManager,RenderContext& renderContext,VkFormat offscreenColorFormat)
	: device(device), resourceManager(resourceManager),renderContext(renderContext),offscreenColorFormat(offscreenColorFormat)
{
	createDescriptorSetLayout();
	createRasterPipeline();
	createUniformBuffer();
	createObjDescriptionBuffer();
	updateDescriptorSet();


}

GraphicsPipelineBuilder::~GraphicsPipelineBuilder()
{

}


void GraphicsPipelineBuilder::createDescriptorSetLayout()
{
	auto nbTxt = static_cast<uint32_t>(resourceManager.textures.size());

	// Camera matrices 
	descSetBindings.addBinding(SceneBindings::eGlobals,  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
											VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR |  VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
	descSetBindings.addBinding(SceneBindings::eObjDescs, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
		                                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
											| VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
	descSetBindings.addBinding(SceneBindings::eTextures, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nbTxt,
											VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
											| VK_SHADER_STAGE_RAYGEN_BIT_KHR |  VK_SHADER_STAGE_ANY_HIT_BIT_KHR );
	descSetBindings.addBinding(SceneBindings::eCubeMap,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1,
											VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
											| VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);

	descSetLayout = descSetBindings.createLayout(device);
	descPool      = descSetBindings.createPool(device,1);
	descSet       = descPool->allocateDescriptorSet(*descSetLayout);

}


void GraphicsPipelineBuilder::createRasterPipeline()
{

	// create raster pipeline !

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

    std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts{descSetLayout};

	rasterPipelineLayout = std::make_unique<PipelineLayout>(device,descSetLayouts,pushConstants);

	rasterRenderPass     = std::make_unique<RenderPass>(device,offscreenColorFormat,  VK_IMAGE_LAYOUT_GENERAL);


	surfaceExtent=renderContext.getSurfaceExtent();
	rasterPipeline = std::make_unique<GraphicPipeline>(rasterShaderModules,*rasterPipelineLayout,device,surfaceExtent);

	rasterPipeline->build(*rasterRenderPass);

}

void GraphicsPipelineBuilder::createUniformBuffer()
{
	globalsBuffer = std::make_unique<Buffer>(device,static_cast<VkDeviceSize>(sizeof(GlobalUniforms)),
		static_cast<VkBufferUsageFlags>( VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));

}

void GraphicsPipelineBuilder::createObjDescriptionBuffer()
{
	objDescBuffer = std::make_unique<Buffer>(device, resourceManager.objDesc, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
}


void GraphicsPipelineBuilder::updateDescriptorSet()
{
	std::vector<VkWriteDescriptorSet> writes;

	// Camera matrices and scene description 
	VkDescriptorBufferInfo dbiUnif{globalsBuffer->getHandle(), 0, VK_WHOLE_SIZE};
	writes.emplace_back(descSetBindings.makeWrite(descSet,SceneBindings::eGlobals,&dbiUnif));

	VkDescriptorBufferInfo dbiSceneDesc{objDescBuffer->getHandle(), 0, VK_WHOLE_SIZE};
	writes.emplace_back(descSetBindings.makeWrite(descSet,SceneBindings::eObjDescs,&dbiSceneDesc));

	// All texture samplers 
	std::vector<VkDescriptorImageInfo> diit;
	for(auto& texture : resourceManager.textures)
	{
		diit.emplace_back(texture.descriptor);
	}
	writes.emplace_back(descSetBindings.makeWriteArray(descSet,SceneBindings::eTextures,diit.data()));

	VkDescriptorImageInfo cubeMapInfo{resourceManager.cubeMapTexture.descriptor};
	writes.emplace_back(descSetBindings.makeWrite(descSet,SceneBindings::eCubeMap,&cubeMapInfo));



	vkUpdateDescriptorSets(device.getHandle(),static_cast<uint32_t>(writes.size()),writes.data(),0,nullptr);
}

// update camera matrix
void GraphicsPipelineBuilder::updateUniformBuffer( CommandBuffer& cmd , Camera& camera ,VkExtent2D surfaceExtent)
{
	// Prepare new UBO contents on host.
	const float aspectRatio = surfaceExtent.width / static_cast<float>(surfaceExtent.height);
	GlobalUniforms hostUBO = {};
	const auto& view = camera.getViewMat();
	auto& proj = glm::perspective(glm::radians(45.0f), (float)surfaceExtent.width / (float)surfaceExtent.height, 0.1f, 1000.0f);
	proj[1][1] *= -1;

	hostUBO.viewProj    = proj * view;
	hostUBO.viewInverse = glm::inverse(view);
	hostUBO.projInverse = glm::inverse(proj);

	// UBO on the device, and what stages access it.
	VkBuffer deviceUBO      = globalsBuffer->getHandle();
	auto     uboUsageStages = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;

	// set lock to ensure the modified UBO is not visible to previous frames.
	VkBufferMemoryBarrier beforeBarrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
	beforeBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	beforeBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	beforeBarrier.buffer        = deviceUBO;
	beforeBarrier.size          = sizeof(hostUBO);
	vkCmdPipelineBarrier(cmd.getHandle(), uboUsageStages, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_DEPENDENCY_DEVICE_GROUP_BIT, 0,
						nullptr, 1, &beforeBarrier, 0, nullptr);

	vkCmdUpdateBuffer(cmd.getHandle(), globalsBuffer->getHandle(), 0, sizeof(GlobalUniforms),&hostUBO);

	// Making sure the updated UBO will be visible
	VkBufferMemoryBarrier afterBarrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
	afterBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	afterBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	afterBarrier.buffer        = deviceUBO;
	afterBarrier.offset        = 0;
	afterBarrier.size          = sizeof(hostUBO);
	vkCmdPipelineBarrier(cmd.getHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, uboUsageStages, VK_DEPENDENCY_DEVICE_GROUP_BIT, 0,
					    nullptr, 1, &afterBarrier, 0, nullptr);
}
