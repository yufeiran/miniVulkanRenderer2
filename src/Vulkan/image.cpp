#include"image.h"
#include"device.h"
namespace mini
{



	Image::Image(VkImage handle, Device& device, const VkExtent2D& extent, VkFormat format, 
		VkImageUsageFlagBits imageUsage, VkSampleCountFlagBits sampleCount):
		device(device),handle(handle),extent(extent),format(format), usage(imageUsage),sampleCount(sampleCount)
	{
	}

	VkImage Image::getHandle() const
	{
		return handle;
	}

	Device& Image::getDevice() const
	{
		return device;
	}

	VkFormat Image::getFormat() const
	{
		return format;
	}

	VkExtent2D Image::getExtent() const
	{
		return extent;
	}

	VkImageUsageFlags Image::getUsage() const
	{
		return usage;
	}

	VkSampleCountFlagBits Image::getSampleCount() const
	{
		return sampleCount;
	}

}