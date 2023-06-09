#pragma once
#include"Vulkan/vk_common.h"
#include"Vulkan/image.h"
#include"Vulkan/imageView.h"

namespace mini
{
class Device;




/* Description of render pass attachments
*  Attachment descriptions can be used to automatically create render target images.
*/
struct Attachment
{
	VkFormat format{ VK_FORMAT_UNDEFINED };

	VkSampleCountFlagBits samples{ VK_SAMPLE_COUNT_1_BIT };

	VkImageUsageFlags usage{ VK_IMAGE_USAGE_SAMPLED_BIT };

	VkImageLayout initialLayout{ VK_IMAGE_LAYOUT_UNDEFINED };

	Attachment() = default;

	Attachment(VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage);
};

/* RenderTarget conatins three vectors for Image , ImageView and Attachment 
*  The first two are Vulkan images and corresponding image view respectively.
*  Attachment (s) contain a dscription of the images, which has two main purposes;
*  - RenderPass creation only needs a list of Attachment (s) , not the actual images, so
*    we keep the minimum amount of information necessary
*  - Creation of a RenderTarget becomes simpler, becaues the caller can just ask for some 
*    Attachment (s) without having to create the images
*  为了方便渲染时候使用的图片？只需要往里面填image或者imageView 它就会自动计算出Attachment和没填的image或者imageView
*/


class RenderTarget
{
public:
using CreateFunc = std::function <std::unique_ptr<RenderTarget>(Image&&)>;

static const CreateFunc DEFAULT_CREATE_FUNC;

RenderTarget(std::vector<Image>&& images);

RenderTarget(std::vector<ImageView>&& imageViews);

const VkExtent2D& getExtent() const;

const std::vector<ImageView>& getViews() const;


private:

Device& device;

VkExtent2D extent{};

std::vector<Image>images;

std::vector<ImageView>views;

std::vector<Attachment> attachments;



};
}