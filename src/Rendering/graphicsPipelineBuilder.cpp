#include <random>

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
		subpassInfo.output.push_back(1);
		subpassInfo.output.push_back(2);
		subpassInfo.output.push_back(3);
		subpassInfo.output.push_back(4);
		subpassInfo.output.push_back(5);
		subpassInfo.output.push_back(6);
		subpassInfo.output.push_back(9);
		subpassInfo.output.push_back(10);

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

	// SSAO pass
	{
		SubpassInfo subpassInfo = {};


		subpassInfo.output.push_back(7);

		VkSubpassDependency ssaoToSSAOBlurDependency = {};

		ssaoToSSAOBlurDependency.srcSubpass = 2;
		ssaoToSSAOBlurDependency.dstSubpass = 3;
		ssaoToSSAOBlurDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		ssaoToSSAOBlurDependency.srcAccessMask = 0;
		ssaoToSSAOBlurDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		ssaoToSSAOBlurDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		subpassInfo.dependencies.push_back(ssaoToSSAOBlurDependency);

		subpassInfos.push_back(subpassInfo);


	}

	// SSAO blur pass
	{
		SubpassInfo subpassInfo = {};

		subpassInfo.output.push_back(8);

		VkSubpassDependency ssaoBlurToLightingDependency = {};

		ssaoBlurToLightingDependency.srcSubpass = 3;
		ssaoBlurToLightingDependency.dstSubpass = 4;
		ssaoBlurToLightingDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		ssaoBlurToLightingDependency.srcAccessMask = 0;
		ssaoBlurToLightingDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		ssaoBlurToLightingDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		subpassInfo.dependencies.push_back(ssaoBlurToLightingDependency);

		subpassInfos.push_back(subpassInfo);
	}



	// lighting pass
	{

		SubpassInfo subpassInfo = {};

		subpassInfo.input.push_back(2);
		subpassInfo.input.push_back(3);
		subpassInfo.input.push_back(4);
		subpassInfo.input.push_back(5);
		subpassInfo.input.push_back(6);
		subpassInfo.input.push_back(7);
		subpassInfo.input.push_back(8);
		subpassInfo.input.push_back(9);
		subpassInfo.input.push_back(10);
		subpassInfo.input.push_back(1);


		subpassInfo.output.push_back(0);

		VkSubpassDependency lightDependency = {};

		lightDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		lightDependency.dstSubpass = 4;
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
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

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
		albedoSpecAttachment.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
		albedoSpecAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(albedoSpecAttachment);

		Attachment metalRoughAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		metalRoughAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		metalRoughAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(metalRoughAttachment);

		Attachment emissionAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		emissionAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		emissionAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(emissionAttachment);

		Attachment ssaoAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		ssaoAttachment.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
		ssaoAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(ssaoAttachment);

		Attachment ssaoBlurAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		ssaoBlurAttachment.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
		ssaoBlurAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(ssaoBlurAttachment);

		Attachment posViewSpaceAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		posViewSpaceAttachment.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
		posViewSpaceAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(posViewSpaceAttachment);

		Attachment normalViewSpaceAttachment{ offscreenColorFormat,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
		normalViewSpaceAttachment.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
		normalViewSpaceAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

		attachments.push_back(normalViewSpaceAttachment);



	}

	std::vector<LoadStoreInfo> loadStoreInfos;
	{
		LoadStoreInfo colorLoadStoreInfo{};
		colorLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(colorLoadStoreInfo);

		LoadStoreInfo depthLoadStoreInfo{};
		depthLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

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

		LoadStoreInfo emissionLoadStoreInfo{};

		emissionLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		emissionLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(emissionLoadStoreInfo);

		LoadStoreInfo ssaoLoadStoreInfo{};

		ssaoLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		ssaoLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(ssaoLoadStoreInfo);

		LoadStoreInfo ssaoBlurLoadStoreInfo{};

		ssaoBlurLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		ssaoBlurLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(ssaoBlurLoadStoreInfo);

		LoadStoreInfo posViewSpaceLoadStoreInfo{};
		posViewSpaceLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		posViewSpaceLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(posViewSpaceLoadStoreInfo);

		LoadStoreInfo normalViewSpaceLoadStoreInfo{};
		normalViewSpaceLoadStoreInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		normalViewSpaceLoadStoreInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		loadStoreInfos.push_back(normalViewSpaceLoadStoreInfo);

	}



	rasterRenderPass = std::make_unique<RenderPass>(device, attachments, loadStoreInfos, subpassInfos);

	surfaceExtent = renderContext.getSurfaceExtent();

	skyLightRenderPass = std::make_unique<SkyLightRenderPass>(device, resourceManager, *rasterRenderPass, surfaceExtent, descSetLayout, pcRaster, 0);


	geomRenderPass = std::make_unique<GeometryRenderPass>(device, resourceManager, *rasterRenderPass, surfaceExtent, descSetLayout, pcRaster, 1);



	ssaoRenderPass = std::make_unique<SSAORenderPass>(device, resourceManager, *rasterRenderPass, surfaceExtent, descSetLayout, ssaoDescSetLayout, pcRaster, 2);

	ssaoBlurRenderPass = std::make_unique<SSAOBlurRenderPass>(device, resourceManager, *rasterRenderPass, surfaceExtent, ssaoBlurDescSetLayout, pcRaster, 3);


	lightingRenderPass = std::make_unique<LightingRenderPass>(device, resourceManager, *rasterRenderPass, surfaceExtent, descSetLayout, gBufferDescSetLayout, pcRaster, 4);



	//createRasterPipeline();
	createUniformBuffer();
	createSSAOBuffers();
	createLightUniformsBuffer();
	createObjDescriptionBuffer();



}

GraphicsPipelineBuilder::~GraphicsPipelineBuilder()
{

}

void GraphicsPipelineBuilder::rebuild(VkExtent2D surfaceExtent)
{
	this->surfaceExtent = surfaceExtent;
	lightingRenderPass->rebuild(surfaceExtent, 4);
	ssaoBlurRenderPass->rebuild(surfaceExtent, 3);
	ssaoRenderPass->rebuild(surfaceExtent, 2);
	geomRenderPass->rebuild(surfaceExtent, 1);
	skyLightRenderPass->rebuild(surfaceExtent, 0);
}

void GraphicsPipelineBuilder::draw(CommandBuffer& cmd)
{

	skyLightRenderPass->draw(cmd, { descSet });

	cmd.nextSubpass();


	geomRenderPass->draw(cmd, { descSet });


	cmd.nextSubpass();

	if (pcRaster.needSSAO == true)
	{
		ssaoRenderPass->draw(cmd, { descSet,ssaoDescSet });

		cmd.nextSubpass();

		ssaoBlurRenderPass->draw(cmd, { ssaoBlurDescSet });

		cmd.nextSubpass();
	}
	else {
		cmd.nextSubpass();
		cmd.nextSubpass();

	}



	lightingRenderPass->draw(cmd, { descSet,gBufferDescSet });
}

void GraphicsPipelineBuilder::update(CommandBuffer& cmd, Camera& camera, VkExtent2D surfaceExtent, const std::vector<Light>& lights)
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

	descSetBindings.addBinding(SceneBindings::eDiffuseIrradiance, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
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
	gBufferDescSetBindings.addBinding(GBufferType::eGEmission, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	gBufferDescSetBindings.addBinding(GBufferType::eGSSAO, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	gBufferDescSetBindings.addBinding(GBufferType::eGSSAOBlur, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	gBufferDescSetBindings.addBinding(GBufferType::eGPositionViewSpace, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	gBufferDescSetBindings.addBinding(GBufferType::eGNormalViewSpace, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	gBufferDescSetBindings.addBinding(GBufferType::eGDepth, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT);



	gBufferDescSetLayout = gBufferDescSetBindings.createLayout(device);
	gBufferDescPool = gBufferDescSetBindings.createPool(device, 1);
	gBufferDescSet = gBufferDescPool->allocateDescriptorSet(*gBufferDescSetLayout);

	// SSAO
	ssaoDescSetBindings.addBinding(SSAOBindings::eSSAOSAMPLE, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	ssaoDescSetBindings.addBinding(SSAOBindings::eSSAONoise, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	ssaoDescSetBindings.addBinding(SSAOBindings::eSSAOPosition, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	ssaoDescSetBindings.addBinding(SSAOBindings::eSSAONormal, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	ssaoDescSetBindings.addBinding(SSAOBindings::eSSAOAlbedo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

	ssaoDescSetLayout = ssaoDescSetBindings.createLayout(device);
	ssaoDescPool = ssaoDescSetBindings.createPool(device, 1);
	ssaoDescSet = ssaoDescPool->allocateDescriptorSet(*ssaoDescSetLayout);

	// SSAO Blur
	ssaoBlurDescSetBindings.addBinding(SSAOBlurBindings::eSSAOInput, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
	ssaoBlurDescSetLayout = ssaoBlurDescSetBindings.createLayout(device);
	ssaoBlurDescPool = ssaoBlurDescSetBindings.createPool(device, 1);
	ssaoBlurDescSet = ssaoBlurDescPool->allocateDescriptorSet(*ssaoBlurDescSetLayout);



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



void GraphicsPipelineBuilder::createSSAOBuffers()
{
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	// Sample kernel

	std::vector<glm::vec3> ssaoKernel;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	ssaoSamplesBuffer = std::make_unique<Buffer>(device, ssaoKernel, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}

	VkExtent2D extent = { 4, 4 };
	ssaoNoiseImage = std::make_unique<Image>(device, extent, ssaoNoise.size() * sizeof(glm::vec3), ssaoNoise.data(), VK_FORMAT_R8G8B8A8_UNORM);
	ssaoNoiseImageView = std::make_unique<ImageView>(*ssaoNoiseImage, VK_FORMAT_R8G8B8A8_UNORM);

}


void GraphicsPipelineBuilder::updateDescriptorSet(RenderTarget& dirShadowRenderTarget, RenderTarget& pointShadowRenderTarget, RenderTarget& offscreenRenderTarget,
	const ImageView& cubemapImageView, const ImageView& diffuseIrradianceImageView)
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




	VkDescriptorImageInfo cubeMapInfo;
	cubeMapInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	cubeMapInfo.imageView = cubemapImageView.getHandle();
	cubeMapInfo.sampler = resourceManager.getDefaultSampler().getHandle();
	writes.emplace_back(descSetBindings.makeWrite(descSet, SceneBindings::eCubeMap, &cubeMapInfo));

	VkDescriptorImageInfo diffuseIrradianceImageInfo;
	diffuseIrradianceImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	diffuseIrradianceImageInfo.imageView = diffuseIrradianceImageView.getHandle();
	diffuseIrradianceImageInfo.sampler = resourceManager.getDefaultSampler().getHandle();
	writes.emplace_back(descSetBindings.makeWrite(descSet, SceneBindings::eDiffuseIrradiance, &diffuseIrradianceImageInfo));


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



	const int GBufferCount = 10;
	std::array<VkDescriptorImageInfo, GBufferCount> descriptors{};

	// Position

	descriptors[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// ָ�� Input attachment ��Դ��ͼ
	descriptors[0].imageView = offscreenRenderTarget.getImageViewByIndex(2).getHandle();
	descriptors[0].sampler = resourceManager.getDefaultSampler().getHandle();

	// Normal
	descriptors[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// ָ�� Input attachment ��Դ��ͼ
	descriptors[1].imageView = offscreenRenderTarget.getImageViewByIndex(3).getHandle();;
	descriptors[1].sampler = resourceManager.getDefaultSampler().getHandle();

	// Albedo
	descriptors[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// ָ�� Input attachment ��Դ��ͼ
	descriptors[2].imageView = offscreenRenderTarget.getImageViewByIndex(4).getHandle();;
	descriptors[2].sampler = resourceManager.getDefaultSampler().getHandle();

	// MetalRough
	descriptors[3].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// ָ�� Input attachment ��Դ��ͼ
	descriptors[3].imageView = offscreenRenderTarget.getImageViewByIndex(5).getHandle();;
	descriptors[3].sampler = resourceManager.getDefaultSampler().getHandle();

	// emssion
	descriptors[4].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// ָ�� Input attachment ��Դ��ͼ
	descriptors[4].imageView = offscreenRenderTarget.getImageViewByIndex(6).getHandle();
	descriptors[4].sampler = resourceManager.getDefaultSampler().getHandle();

	// SSAO
	descriptors[5].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// ָ�� Input attachment ��Դ��ͼ
	descriptors[5].imageView = offscreenRenderTarget.getImageViewByIndex(7).getHandle();
	descriptors[5].sampler = resourceManager.getDefaultSampler().getHandle();

	// SSAO Blur 
	descriptors[6].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// ָ�� Input attachment ��Դ��ͼ
	descriptors[6].imageView = offscreenRenderTarget.getImageViewByIndex(8).getHandle();
	descriptors[6].sampler = resourceManager.getDefaultSampler().getHandle();

	// Position in view space

	descriptors[7].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// ָ�� Input attachment ��Դ��ͼ
	descriptors[7].imageView = offscreenRenderTarget.getImageViewByIndex(9).getHandle();
	descriptors[7].sampler = resourceManager.getDefaultSampler().getHandle();

	// Normal
	descriptors[8].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// ָ�� Input attachment ��Դ��ͼ
	descriptors[8].imageView = offscreenRenderTarget.getImageViewByIndex(10).getHandle();;
	descriptors[8].sampler = resourceManager.getDefaultSampler().getHandle();


	// Depth
	descriptors[9].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// ָ�� Input attachment ��Դ��ͼ
	descriptors[9].imageView = offscreenRenderTarget.getImageViewByIndex(1).getHandle();;
	descriptors[9].sampler = resourceManager.getDefaultSampler().getHandle();




	std::array<VkWriteDescriptorSet, GBufferCount> writeDescriptorSets{};

	for (int i = 0; i < GBufferCount; i++)
	{
		writeDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[i].dstSet = gBufferDescSet;
		writeDescriptorSets[i].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		writeDescriptorSets[i].descriptorCount = 1;
		writeDescriptorSets[i].dstBinding = i;
		writeDescriptorSets[i].pImageInfo = &descriptors[i];
	}


	// ����Դ�󶨵���������
	vkUpdateDescriptorSets(device.getHandle(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);


	// SSAO
	VkDescriptorBufferInfo ssaoSampleBufferInfo{ ssaoSamplesBuffer->getHandle(), 0, VK_WHOLE_SIZE };
	std::vector<VkWriteDescriptorSet> ssaoWrites;

	VkDescriptorImageInfo ssaoNoiseInfo;
	ssaoNoiseInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	ssaoNoiseInfo.imageView = ssaoNoiseImageView->getHandle();
	ssaoNoiseInfo.sampler = resourceManager.getRepeatSampler().getHandle();

	VkDescriptorImageInfo gBufferPosInfo;
	gBufferPosInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	gBufferPosInfo.imageView = offscreenRenderTarget.getImageViewByIndex(9).getHandle();
	gBufferPosInfo.sampler = resourceManager.getRepeatSampler().getHandle();

	VkDescriptorImageInfo gBufferNormalInfo;
	gBufferNormalInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	gBufferNormalInfo.imageView = offscreenRenderTarget.getImageViewByIndex(10).getHandle();
	gBufferNormalInfo.sampler = resourceManager.getRepeatSampler().getHandle();

	VkDescriptorImageInfo gBufferAlbedoInfo;
	gBufferAlbedoInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	gBufferAlbedoInfo.imageView = offscreenRenderTarget.getImageViewByIndex(4).getHandle();
	gBufferAlbedoInfo.sampler = resourceManager.getRepeatSampler().getHandle();

	ssaoWrites.emplace_back(ssaoDescSetBindings.makeWrite(ssaoDescSet, SSAOBindings::eSSAOSAMPLE, &ssaoSampleBufferInfo));
	ssaoWrites.emplace_back(ssaoDescSetBindings.makeWrite(ssaoDescSet, SSAOBindings::eSSAONoise, &ssaoNoiseInfo));

	ssaoWrites.emplace_back(ssaoDescSetBindings.makeWrite(ssaoDescSet, SSAOBindings::eSSAOPosition, &gBufferPosInfo));
	ssaoWrites.emplace_back(ssaoDescSetBindings.makeWrite(ssaoDescSet, SSAOBindings::eSSAONormal, &gBufferNormalInfo));
	ssaoWrites.emplace_back(ssaoDescSetBindings.makeWrite(ssaoDescSet, SSAOBindings::eSSAOAlbedo, &ssaoNoiseInfo));


	vkUpdateDescriptorSets(device.getHandle(), static_cast<uint32_t>(ssaoWrites.size()), ssaoWrites.data(), 0, nullptr);

	// SSAO Blur
	std::vector<VkWriteDescriptorSet> ssaoBlurWrites;

	VkDescriptorImageInfo ssaoInputInfo;
	ssaoInputInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	ssaoInputInfo.imageView = offscreenRenderTarget.getImageViewByIndex(7).getHandle();
	ssaoInputInfo.sampler = resourceManager.getDefaultSampler().getHandle();
	ssaoBlurWrites.emplace_back(ssaoBlurDescSetBindings.makeWrite(ssaoBlurDescSet, SSAOBlurBindings::eSSAOInput, &ssaoInputInfo));

	vkUpdateDescriptorSets(device.getHandle(), static_cast<uint32_t>(ssaoBlurWrites.size()), ssaoBlurWrites.data(), 0, nullptr);




}

// update camera matrix
void GraphicsPipelineBuilder::updateUniformBuffer(CommandBuffer& cmd, Camera& camera, VkExtent2D surfaceExtent)
{
	// Prepare new UBO contents on host.
	const float aspectRatio = surfaceExtent.width / static_cast<float>(surfaceExtent.height);
	GlobalUniforms hostUBO = {};
	const auto& view = camera.getViewMat();
	auto proj = glm::perspective(glm::radians(45.0f), (float)surfaceExtent.width / (float)surfaceExtent.height, camera.getNearPlane(), camera.getFarPlane());
	proj[1][1] *= -1;

	hostUBO.viewProj = proj * view;
	hostUBO.view = view;
	hostUBO.viewNoTranslate = glm::mat4(glm::mat3(view));
	hostUBO.proj = proj;
	hostUBO.viewInverse = glm::inverse(view);
	hostUBO.projInverse = glm::inverse(proj);
	hostUBO.farPlane = camera.getFarPlane();
	hostUBO.nearPlane = camera.getNearPlane();

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
