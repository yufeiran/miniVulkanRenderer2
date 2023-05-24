#include"image.h"
#include"device.h"
namespace mini
{



	Image::Image(VkImage handle, Device& device, const VkExtent2D& extent, VkFormat format):
		device(device),handle(handle),extent(extent),format(format)
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

}