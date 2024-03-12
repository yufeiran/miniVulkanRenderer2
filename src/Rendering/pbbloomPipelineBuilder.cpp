#include "pbbloomPipelineBuilder.h"

using namespace mini;

pbbloomPipelineBuilder::pbbloomPipelineBuilder(
	Device& device, 
	ResourceManager& resourceManager,
	VkExtent2D extent, 
	std::shared_ptr<DescriptorSetLayout> descSetLayout, 
	RenderTarget& gBufferRenderTarget, 
	VkFormat offscreenColorFormat, 
	PushConstantRaster& pcRaster,
	int N)
	: device(device),
	resourceManager(resourceManager),
	pcRaster(pcRaster),
	gBufferRenderTarget(gBufferRenderTarget),N(N)
{




	createRenderTarget();
}

pbbloomPipelineBuilder::~pbbloomPipelineBuilder()
{
}

void pbbloomPipelineBuilder::draw(CommandBuffer& cmd, const VkDescriptorSet& descSet)
{
}

void pbbloomPipelineBuilder::createRenderTarget()
{

	auto originalExtent = gBufferRenderTarget.getExtent();


	std::vector<Image> images;
	for(int i = 0; i < N; i++)
	{
		originalExtent.width /= 2;
		originalExtent.height /= 2;
		std::unique_ptr<Image> bloomImage = std::make_unique<Image>(device,
						originalExtent, offscreenColorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);

		images.push_back(std::move(*bloomImage));
	}

	renderTarget = std::make_unique<RenderTarget>(std::move(images));

	framebuffer = std::make_unique<FrameBuffer>(device,*renderTarget,*renderPass);


}
