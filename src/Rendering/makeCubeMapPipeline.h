
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

class Device;
class ResourceManager;
class DescriptorSetBindings;
class DescriptorSetLayout;
class DescriptorPool;


class MakeCubeMapPipeline
{
public:
	MakeCubeMapPipeline(Device& device, ResourceManager& resourceManager,PushConstantRaster& pcRaster);
	~MakeCubeMapPipeline();

	void createHDRToCubeMapUniforms();

	void createDescriptorSetLayout();
	void createRenderTarget();

	void updateDescriptorSet(ImageView& hdrImageView);


	void draw(CommandBuffer& cmd);

	RenderTarget& getCubeMapRenderTarget() { return *hdrToCubeMapRenderTarget; }

	RenderTarget& getDiffuseIrradianceRenderTarget() { return *diffuseIrradianceRenderTarget; }


private:

	const uint32_t CUBE_MAP_SIZE = 2048;
	const VkFormat CUBE_MAP_FORMAT = VK_FORMAT_R8G8B8A8_SRGB ;

	const VkExtent2D cubeMapExtent = { CUBE_MAP_SIZE,CUBE_MAP_SIZE };
	const VkExtent2D diffuseIrradianceExtent = { CUBE_MAP_SIZE / 64,CUBE_MAP_SIZE / 64 };
	//const VkExtent2D diffuseIrradianceExtent = { CUBE_MAP_SIZE / 64,CUBE_MAP_SIZE / 64 };

	Device&                               device;		
	ResourceManager&                      resourceManager;

	std::unique_ptr<PipelineLayout>       pipelineLayout;

	// HDR to CubeMap
	DescriptorSetBindings                 hdrToCubeMapDescSetBindings;
	std::shared_ptr<DescriptorSetLayout>  hdrToCubeMapDescSetLayout;
	std::unique_ptr<DescriptorPool>       hdrToCubeMapDescPool;
	VkDescriptorSet                       hdrToCubeMapDescSet;


	std::unique_ptr<RenderPass>           hdrToCubeMapRenderPass;

	std::unique_ptr<RenderTarget>         hdrToCubeMapRenderTarget;
	std::unique_ptr<FrameBuffer>          hdrToCubeMapFramebuffer;

	std::unique_ptr<HDRToCubeMapRenderPass>       hdrToCubeMapPass;

	// Diffuse Irradiance
	DescriptorSetBindings                 diffuseIrradianceDescSetBindings;
	std::shared_ptr<DescriptorSetLayout>  diffuseIrradianceDescSetLayout;
	std::unique_ptr<DescriptorPool>       diffuseIrradianceDescPool;
	VkDescriptorSet                       diffuseIrradianceDescSet;

	std::unique_ptr<RenderPass>           diffuseIrradianceRenderPass;

	std::unique_ptr<RenderTarget>         diffuseIrradianceRenderTarget;
	std::unique_ptr<FrameBuffer>          diffuseIrradianceFramebuffer;


	std::unique_ptr<DiffuseIrradianceRenderPass>  diffuseIrradiancePass;



	PushConstantRaster&                   pcRaster;

	std::unique_ptr<Buffer>               hdrToCubeMapUniforms;
	HDRToCubeMapUniforms				  hdrToCubeMapUniformsData;
};

}