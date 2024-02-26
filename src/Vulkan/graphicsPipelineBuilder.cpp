#include "Vulkan/graphicsPipelineBuilder.h"
#include "ResourceManagement/ResourceManager.h"
#include "Common/camera.h"
#include "Vulkan/commandBuffer.h"
#include "Vulkan/buffer.h"


using namespace mini;

GraphicsPipelineBuilder::GraphicsPipelineBuilder(Device& device, 
	ResourceManager& resourceManager,
	RenderContext& renderContext,
	VkFormat offscreenColorFormat,
	PushConstantRaster& pcRaster)
	: device(device), resourceManager(resourceManager),renderContext(renderContext),offscreenColorFormat(offscreenColorFormat),pcRaster(pcRaster)
{


	std::vector<SubpassInfo> subpassInfos;


	{
	
		SubpassInfo subpassInfo = {};
		subpassInfo.output.push_back(0);
		subpassInfo.output.push_back(1);

		VkSubpassDependency skyLightToForwardDependency = {};
		skyLightToForwardDependency.srcSubpass = 0;
		skyLightToForwardDependency.dstSubpass = 1;
		skyLightToForwardDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		skyLightToForwardDependency.srcAccessMask = 0;
		skyLightToForwardDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		skyLightToForwardDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		subpassInfo.dependencies.push_back(skyLightToForwardDependency);

		subpassInfos.push_back(subpassInfo);

	}

	{

		SubpassInfo subpassInfo = {};
		subpassInfo.output.push_back(0);
		subpassInfo.output.push_back(1);

		VkSubpassDependency forwardDependency = {};

		forwardDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		forwardDependency.dstSubpass = 1;
		forwardDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		forwardDependency.srcAccessMask = 0;
		forwardDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		forwardDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


		subpassInfo.dependencies.push_back(forwardDependency);

		subpassInfos.push_back(subpassInfo);

	}



	 
	createDescriptorSetLayout();

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = offscreenColorFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = device.getPhysicalDevice().findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::vector<Attachment> attachments;
	{
		Attachment colorAttachment{offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT};
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout   = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(colorAttachment);

		Attachment depthAttachment{device.getPhysicalDevice().findDepthFormat(),VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT};
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachments.push_back(depthAttachment);
	}

	std::vector<LoadStoreInfo> loadStoreInfos;
	{
		LoadStoreInfo colorLoadStoreInfo{};
		colorLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	
		loadStoreInfos.push_back(colorLoadStoreInfo);
	
		LoadStoreInfo depthLoadStoreInfo{};
		depthLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	
		loadStoreInfos.push_back(depthLoadStoreInfo);
	}



	rasterRenderPass     = std::make_unique<RenderPass>(device,attachments,loadStoreInfos,subpassInfos);


	surfaceExtent = renderContext.getSurfaceExtent();

	forwardRenderPass = std::make_unique<ForwardRenderPass>(device,resourceManager,*rasterRenderPass,surfaceExtent,descSetLayout,pcRaster);

	skyLightRenderPass = std::make_unique<SkyLightRenderPass>(device,resourceManager,*rasterRenderPass,surfaceExtent,descSetLayout,pcRaster);

	//createRasterPipeline();
	createUniformBuffer();
	createObjDescriptionBuffer();	
	updateDescriptorSet();


}

GraphicsPipelineBuilder::~GraphicsPipelineBuilder()
{

}

void GraphicsPipelineBuilder::rebuild(VkExtent2D surfaceExtent)
{
	this->surfaceExtent = surfaceExtent;
	forwardRenderPass->rebuild(surfaceExtent);
	skyLightRenderPass->rebuild(surfaceExtent);
}

void GraphicsPipelineBuilder::draw(CommandBuffer& cmd)
{

	skyLightRenderPass->draw(cmd,descSet);

	cmd.nextSubpass();

	forwardRenderPass->draw(cmd,descSet);
}

void GraphicsPipelineBuilder::update(CommandBuffer& cmd,Camera& camera, VkExtent2D surfaceExtent)
{
	updateUniformBuffer(cmd,camera,surfaceExtent);
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

//
//void GraphicsPipelineBuilder::createRasterPipeline()
//{
//
//	// create raster pipeline !
//
//	rasterShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/vertexShader.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT));
//	rasterShaderModules.push_back(std::make_unique<ShaderModule>("../../spv/fragmentShader.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT));
//	
//	std::vector<VkPushConstantRange> pushConstants;
//	VkPushConstantRange pushConstant={};
//	pushConstant.offset = 0;
//	pushConstant.size = sizeof(PushConstantRaster);
//	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
//	pushConstants.push_back(pushConstant);
//
//	//VkDescriptorSetLayoutBinding uboLayoutBinding{};
//	//uboLayoutBinding.binding = 0;
//	//uboLayoutBinding.descriptorCount = 1;
//	//uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	//uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//	//uboLayoutBinding.pImmutableSamplers = nullptr;
//
//	//VkDescriptorSetLayoutBinding samplerLayoutBinding{};
//	//samplerLayoutBinding.binding = 1;
//	//samplerLayoutBinding.descriptorCount=1;
//	//samplerLayoutBinding.descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//	//samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//	//samplerLayoutBinding.pImmutableSamplers = nullptr;
//
//	//std::vector<VkDescriptorSetLayoutBinding>layoutBindings{uboLayoutBinding,samplerLayoutBinding};
//
//
//
//	//rasterDescriptorSetLayouts.push_back(std::make_unique<DescriptorSetLayout>(*device,layoutBindings));
//
//    std::vector<std::shared_ptr<DescriptorSetLayout>>  descSetLayouts{descSetLayout};
//
//	rasterPipelineLayout = std::make_unique<PipelineLayout>(device,descSetLayouts,pushConstants);
//
//
//	surfaceExtent=renderContext.getSurfaceExtent();
//	rasterPipeline = std::make_unique<GraphicsPipeline>(rasterShaderModules,*rasterPipelineLayout,device,surfaceExtent);
//
//	rasterPipeline->build(*rasterRenderPass);
//
//}

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

	hostUBO.viewProj        = proj * view;
	hostUBO.view            = view;
	hostUBO.viewNoTranslate = glm::mat4(glm::mat3(view));
	hostUBO.proj            = proj;
	hostUBO.viewInverse     = glm::inverse(view);
	hostUBO.projInverse     = glm::inverse(proj);

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
