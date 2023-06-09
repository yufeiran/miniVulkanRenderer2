#include"imageView.h"
#include"image.h"
#include"device.h"
namespace mini
{

ImageView::ImageView(Image& image ,VkFormat format, VkImageViewType viewType)
	:device(image.getDevice()),format(format),image{image}
{
	if (format == VK_FORMAT_UNDEFINED)
	{
		format = image.getFormat();
		this->format = image.getFormat();
	}

	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image.getHandle();
	createInfo.viewType = viewType;
	createInfo.format = format;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	if (format == VK_FORMAT_D32_SFLOAT|| format == VK_FORMAT_D32_SFLOAT_S8_UINT||format==VK_FORMAT_D24_UNORM_S8_UINT) {
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

	}
	else if(format== VK_FORMAT_R8G8B8A8_SRGB||format==VK_FORMAT_B8G8R8A8_SRGB){
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	else {
		throw Error("Unsupport format!");
	}

	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	if (vkCreateImageView(device.getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
		throw Error("failed to create image views");
	}


}

ImageView::~ImageView()
{
	if (handle != VK_NULL_HANDLE) {
		vkDestroyImageView(device.getHandle(), handle, nullptr);
	}
}

ImageView::ImageView(ImageView&& other) :
	handle{ other.handle },
	device{ other.device },
	format{ other.format },
	image{ other.image }

{
	other.handle = VK_NULL_HANDLE;
}

Device& ImageView::getDevice() const
{
	return device;
}

Image& ImageView::getImage() const
{
	return image;
}

VkImageView ImageView::getHandle() const
{
	return handle;
}

}