#include "Rendering/graphicsPipelineBuilder.h"
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
	: device(device), resourceManager(resourceManager), renderContext(renderContext), offscreenColorFormat(offscreenColorFormat), pcRaster(pcRaster)
{


	std::vector<SubpassInfo> subpassInfos;


	// skylight pass
	{

		SubpassInfo subpassInfo = {};
		subpassInfo.output.push_back(0);
		subpassInfo.output.push_back(1);

		VkSubpassDependency skyLightToForwardDependency = {};
		skyLightToForwardDependency.srcSubpass = 0;
		skyLightToForwardDependency.dstSubpass = 1;
		skyLightToForwardDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		skyLightToForwardDependency.srcAccessMask = 0;
		skyLightToForwardDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		skyLightToForwardDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		subpassInfo.dependencies.push_back(skyLightToForwardDependency);

		subpassInfos.push_back(subpassInfo);

	}


	// geom render pass
	{

		SubpassInfo subpassInfo = {};
		subpassInfo.output.push_back(0);
		subpassInfo.output.push_back(1);
		subpassInfo.output.push_back(2);
		subpassInfo.output.push_back(3);
		subpassInfo.output.push_back(4);
		subpassInfo.output.push_back(5);

		VkSubpassDependency geomToLightingDependency = {};

		geomToLightingDependency.srcSubpass = 1;
		geomToLightingDependency.dstSubpass = 2;
		geomToLightingDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		geomToLightingDependency.srcAccessMask = 0;
		geomToLightingDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		geomToLightingDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


		subpassInfo.dependencies.push_back(geomToLightingDependency);

		subpassInfos.push_back(subpassInfo);


	}

	{

		SubpassInfo subpassInfo = {};

		subpassInfo.input.push_back(2);
		subpassInfo.input.push_back(3);
		subpassInfo.input.push_back(4);
		subpassInfo.input.push_back(5);
		subpassInfo.input.push_back(1);

		subpassInfo.output.push_back(0);
		subpassInfo.output.push_back(1);

		VkSubpassDependency lightDependency = {};

		lightDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		lightDependency.dstSubpass = 2;
		lightDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		lightDependency.srcAccessMask = 0;
		lightDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		lightDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


		subpassInfo.dependencies.push_back(lightDependency);

		subpassInfos.push_back(subpassInfo);

	}



	createDescriptorSetLayout();


	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::vector<Attachment> attachments;
	{
		Attachment colorAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(colorAttachment);

		Attachment depthAttachment{ device.getPhysicalDevice().findDepthFormat(),VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT };
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachments.push_back(depthAttachment);

		Attachment posAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		posAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		posAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(posAttachment);

		Attachment normalAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		normalAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		normalAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(normalAttachment);

		Attachment albedoSpecAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		albedoSpecAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		albedoSpecAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(albedoSpecAttachment);

		Attachment metalRoughAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		metalRoughAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		metalRoughAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(metalRoughAttachment);


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

		LoadStoreInfo posLoadStoreInfo{};
		posLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		posLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(posLoadStoreInfo);

		LoadStoreInfo normalLoadStoreInfo{};
		normalLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		normalLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(normalLoadStoreInfo);

		LoadStoreInfo albedoSpecLoadStoreInfo{};

		albedoSpecLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		albedoSpecLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(albedoSpecLoadStoreInfo);

		LoadStoreInfo metalRoughLoadStoreInfo{};

		metalRoughLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		metalRoughLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(metalRoughLoadStoreInfo);

	}



	rasterRenderPass = std::make_unique<RenderPass>(device, attachments, loadStoreInfos, subpassInfos);

	surfaceExtent = renderContext.getSurfaceExtent();

	geomRenderPass = std::make_unique<GeometryRenderPass>(device, resourceManager, *rasterRenderPass, surfaceExtent, descSetLayout, pcRaster, 1);


	lightingRenderPass = std::make_unique<LightingRenderPass>(device, resourceManager, *rasterRenderPass, surfaceExtent, descSetLayout,gBufferDescSetLayout, pcRaster, 2);

	skyLightRenderPass = std::make_unique<SkyLightRenderPass>(device, resourceManager, *rasterRenderPass, surfaceExtent, descSetLayout, pcRaster, 0);

	//createRasterPipeline();
	createUniformBuffer();
	createLightUniformsBuffer();
	createObjDescriptionBuffer();


}

GraphicsPipelineBuilder::~GraphicsPipelineBuilder()
{

}

void GraphicsPipelineBuilder::rebuild(VkExtent2D surfaceExtent)
{
	this->surfaceExtent = surfaceExtent;
	geomRenderPass->rebuild(surfaceExtent, 1);
	skyLightRenderPass->rebuild(surfaceExtent, 0);
}

void GraphicsPipelineBuilder::draw(CommandBuffer& cmd)
{

	skyLightRenderPass->draw(cmd, {descSet});

	cmd.nextSubpass();


	geomRenderPass->draw(cmd, {descSet});

	cmd.nextSubpass();

	lightingRenderPass->draw(cmd, {descSet,gBufferDescSet});
}

void GraphicsPipelineBuilder::update(CommandBuffer& cmd, Camera& camera, VkExtent2D surfaceExtent, std::vector<Light>& lights)
{
	updateUniformBuffer(cmd, camera, surfaceExtent);
	updateLightUniformsBuffer(cmd, lights);
}


void GraphicsPipelineBuilder::createDescriptorSetLayout()
{
	auto nbTxt = static_cast<uint32_t>(resourceManager.textures.size());

	// Camera matrices 
	descSetBindings.addBinding(SceneBindings::eGlobals, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
	descSetBindings.addBinding(SceneBindings::eObjDescs, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
		| VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
	descSetBindings.addBinding(SceneBindings::eTextures, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nbTxt,
		VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
		| VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
	descSetBindings.addBinding(SceneBindings::eCubeMap, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
		| VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);

	descSetBindings.addBinding(SceneBindings::eDirShadowMap, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
		| VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);

	descSetBindings.addBinding(SceneBindings::ePointShadowMap, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
		| VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);

	descSetBindings.addBinding(SceneBindings::eLight, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);


	descSetLayout = descSetBindings.createLayout(device);
	descPool = descSetBindings.createPool(device, 1);
	descSet = descPool->allocateDescriptorSet(*descSetLayout);


	// GBuffer
	gBufferDescSetBindings.addBinding(GBufferType::eGPosition, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	gBufferDescSetBindings.addBinding(GBufferType::eGNormal, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	gBufferDescSetBindings.addBinding(GBufferType::eGAlbedo, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	gBufferDescSetBindings.addBinding(GBufferType::eGMetalRough, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	gBufferDescSetBindings.addBinding(GBufferType::eGDepth, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

	gBufferDescSetLayout = gBufferDescSetBindings.createLayout(device);
	gBufferDescPool = gBufferDescSetBindings.createPool(device, 1);
	gBufferDescSet = gBufferDescPool->allocateDescriptorSet(*gBufferDescSetLayout);

}



void GraphicsPipelineBuilder::createUniformBuffer()
{
	globalsBuffer = std::make_unique<Buffer>(device, static_cast<VkDeviceSize>(sizeof(GlobalUniforms)),
		static_cast<VkBufferUsageFlags>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));

}

void GraphicsPipelineBuilder::createObjDescriptionBuffer()
{
	objDescBuffer = std::make_unique<Buffer>(device, resourceManager.objDesc, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
}


void GraphicsPipelineBuilder::updateDescriptorSet(RenderTarget& dirShadowRenderTarget, RenderTarget& pointShadowRenderTarget,RenderTarget& offscreenRenderTarget)
{
	std::vector<VkWriteDescriptorSet> writes;

	// Camera matrices and scene description 
	VkDescriptorBufferInfo dbiUnif{ globalsBuffer->getHandle(), 0, VK_WHOLE_SIZE };
	writes.emplace_back(descSetBindings.makeWrite(descSet, SceneBindings::eGlobals, &dbiUnif));

	VkDescriptorBufferInfo dbiSceneDesc{ objDescBuffer->getHandle(), 0, VK_WHOLE_SIZE };
	writes.emplace_back(descSetBindings.makeWrite(descSet, SceneBindings::eObjDescs, &dbiSceneDesc));

	// All texture samplers 
	std::vector<VkDescriptorImageInfo> diit;
	for (auto& texture : resourceManager.textures)
	{
		diit.emplace_back(texture.descriptor);
	}
	writes.emplace_back(descSetBindings.makeWriteArray(descSet, SceneBindings::eTextures, diit.data()));

	VkDescriptorImageInfo cubeMapInfo{ resourceManager.cubeMapTexture.descriptor };
	writes.emplace_back(descSetBindings.makeWrite(descSet, SceneBindings::eCubeMap, &cubeMapInfo));


	VkDescriptorImageInfo shadowMapInfo;
	shadowMapInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	shadowMapInfo.imageView = dirShadowRenderTarget.getImageViewByIndex(0).getHandle();
	shadowMapInfo.sampler = resourceManager.getDefaultSampler().getHandle();

	writes.emplace_back(descSetBindings.makeWrite(descSet, SceneBindings::eDirShadowMap, &shadowMapInfo));

	VkDescriptorImageInfo pointShadowMapInfo;
	pointShadowMapInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	pointShadowMapInfo.imageView = pointShadowRenderTarget.getImageViewByIndex(0).getHandle();
	pointShadowMapInfo.sampler = resourceManager.getDefaultSampler().getHandle();

	writes.emplace_back(descSetBindings.makeWrite(descSet, SceneBindings::ePointShadowMap, &pointShadowMapInfo));

	VkDescriptorBufferInfo lightUnif{ lightUniformsBuffer->getHandle(), 0, VK_WHOLE_SIZE };
	writes.emplace_back(descSetBindings.makeWrite(descSet, SceneBindings::eLight, &lightUnif));




	vkUpdateDescriptorSets(device.getHandle(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);



	std::array<VkDescriptorImageInfo, 5> descriptors{};

	// Position

	descriptors[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// 指定 Input attachment 资源视图
	descriptors[0].imageView = offscreenRenderTarget.getImageViewByIndex(2).getHandle();
	descriptors[0].sampler = resourceManager.getDefaultSampler().getHandle();

	// Normal
	descriptors[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// 指定 Input attachment 资源视图
	descriptors[1].imageView = offscreenRenderTarget.getImageViewByIndex(3).getHandle();;
	descriptors[1].sampler = resourceManager.getDefaultSampler().getHandle();

	// Albedo
	descriptors[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// 指定 Input attachment 资源视图
	descriptors[2].imageView = offscreenRenderTarget.getImageViewByIndex(4).getHandle();;
	descriptors[2].sampler = resourceManager.getDefaultSampler().getHandle();

	// MetalRough
	descriptors[3].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// 指定 Input attachment 资源视图
	descriptors[3].imageView = offscreenRenderTarget.getImageViewByIndex(5).getHandle();;
	descriptors[3].sampler = resourceManager.getDefaultSampler().getHandle();

	// Depth
	descriptors[4].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// 指定 Input attachment 资源视图
	descriptors[4].imageView = offscreenRenderTarget.getImageViewByIndex(1).getHandle();;
	descriptors[4].sampler = resourceManager.getDefaultSampler().getHandle();



	std::array<VkWriteDescriptorSet, 5> writeDescriptorSets{};

	writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	// 指定根据上面 DescriptorSet layout 创建的 DescriptorSet
	writeDescriptorSets[0].dstSet = gBufferDescSet;
	// 指定描述符类型为 Input Attachment，此处与上一步描述符设置的类型一致
	writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	writeDescriptorSets[0].descriptorCount = 1;
	writeDescriptorSets[0].dstBinding = 0;
	writeDescriptorSets[0].pImageInfo = &descriptors[0];

	writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	// 指定根据上面 DescriptorSet layout 创建的 DescriptorSet
	writeDescriptorSets[1].dstSet = gBufferDescSet;
	// 指定描述符类型为 Input Attachment，此处与上一步描述符设置的描述符类型一致
	writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	writeDescriptorSets[1].descriptorCount = 1;
	writeDescriptorSets[1].dstBinding = 1;
	writeDescriptorSets[1].pImageInfo = &descriptors[1];

	writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSets[2].dstSet = gBufferDescSet;
	// 指定描述符类型为 Input Attachment，此处与上一步描述符设置的描述符类型一致
	writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	writeDescriptorSets[2].descriptorCount = 1;
	writeDescriptorSets[2].dstBinding = 2;
	writeDescriptorSets[2].pImageInfo = &descriptors[2];

	writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSets[3].dstSet = gBufferDescSet;
	// 指定描述符类型为 Input Attachment，此处与上一步描述符设置的描述符类型一致
	writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	writeDescriptorSets[3].descriptorCount = 1;
	writeDescriptorSets[3].dstBinding = 3;
	writeDescriptorSets[3].pImageInfo = &descriptors[3];

	writeDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSets[4].dstSet = gBufferDescSet;
	// 指定描述符类型为 Input Attachment，此处与上一步描述符设置的描述符类型一致
	writeDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	writeDescriptorSets[4].descriptorCount = 1;
	writeDescriptorSets[4].dstBinding = 4;
	writeDescriptorSets[4].pImageInfo = &descriptors[4];

	// 将资源绑定到描述符集
	vkUpdateDescriptorSets(device.getHandle(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);



}

// update camera matrix
void GraphicsPipelineBuilder::updateUniformBuffer(CommandBuffer& cmd, Camera& camera, VkExtent2D surfaceExtent)
{
	// Prepare new UBO contents on host.
	const float aspectRatio = surfaceExtent.width / static_cast<float>(surfaceExtent.height);
	GlobalUniforms hostUBO = {};
	const auto& view = camera.getViewMat();
	auto& proj = glm::perspective(glm::radians(45.0f), (float)surfaceExtent.width / (float)surfaceExtent.height, 0.1f, 1000.0f);
	proj[1][1] *= -1;

	hostUBO.viewProj = proj * view;
	hostUBO.view = view;
	hostUBO.viewNoTranslate = glm::mat4(glm::mat3(view));
	hostUBO.proj = proj;
	hostUBO.viewInverse = glm::inverse(view);
	hostUBO.projInverse = glm::inverse(proj);

	// UBO on the device, and what stages access it.
	VkBuffer deviceUBO = globalsBuffer->getHandle();
	auto     uboUsageStages = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;

	// set lock to ensure the modified UBO is not visible to previous frames.
	VkBufferMemoryBarrier beforeBarrier{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
	beforeBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	beforeBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	beforeBarrier.buffer = deviceUBO;
	beforeBarrier.size = sizeof(hostUBO);
	vkCmdPipelineBarrier(cmd.getHandle(), uboUsageStages, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_DEPENDENCY_DEVICE_GROUP_BIT, 0,
		nullptr, 1, &beforeBarrier, 0, nullptr);

	vkCmdUpdateBuffer(cmd.getHandle(), globalsBuffer->getHandle(), 0, sizeof(GlobalUniforms), &hostUBO);

	// Making sure the updated UBO will be visible
	VkBufferMemoryBarrier afterBarrier{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
	afterBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	afterBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	afterBarrier.buffer = deviceUBO;
	afterBarrier.offset = 0;
	afterBarrier.size = sizeof(hostUBO);
	vkCmdPipelineBarrier(cmd.getHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, uboUsageStages, VK_DEPENDENCY_DEVICE_GROUP_BIT, 0,
		nullptr, 1, &afterBarrier, 0, nullptr);
}

void GraphicsPipelineBuilder::createLightUniformsBuffer()
{
	lightUniformsBuffer = std::make_unique<Buffer>(device, static_cast<VkDeviceSize>(sizeof(LightUniforms)),
		static_cast<VkBufferUsageFlags>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));
}

void GraphicsPipelineBuilder::updateLightUniformsBuffer(CommandBuffer& cmd, const std::vector<Light>& shadowLights)
{
	LightUniforms hostUBO = createLightUniforms(shadowLights);

	// UBO on the device, and what stages access it.
	VkBuffer deviceUBO = lightUniformsBuffer->getHandle();
	auto     uboUsageStages = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;

	// set lock to ensure the modified UBO is not visible to previous frames.
	VkBufferMemoryBarrier beforeBarrier{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
	beforeBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	beforeBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	beforeBarrier.buffer = deviceUBO;
	beforeBarrier.size = sizeof(hostUBO);
	vkCmdPipelineBarrier(cmd.getHandle(), uboUsageStages, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_DEPENDENCY_DEVICE_GROUP_BIT, 0,
		nullptr, 1, &beforeBarrier, 0, nullptr);

	vkCmdUpdateBuffer(cmd.getHandle(), lightUniformsBuffer->getHandle(), 0, sizeof(LightUniforms), &hostUBO);

	// Making sure the updated UBO will be visible
	VkBufferMemoryBarrier afterBarrier{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
	afterBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	afterBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	afterBarrier.buffer = deviceUBO;
	afterBarrier.offset = 0;
	afterBarrier.size = sizeof(hostUBO);
	vkCmdPipelineBarrier(cmd.getHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, uboUsageStages, VK_DEPENDENCY_DEVICE_GROUP_BIT, 0,
		nullptr, 1, &afterBarrier, 0, nullptr);
}
