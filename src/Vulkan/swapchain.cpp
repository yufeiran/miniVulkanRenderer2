#include"swapchain.h"
#include"device.h"
#include"imageView.h"


namespace mini
{

//把usage的flag压缩成一个flag
void Swapchain::compImageUsageFlags()
{
	swapchianUsageFlags = *imageUsageFlags.begin();
	for (auto flag : imageUsageFlags) {
		swapchianUsageFlags =static_cast<VkImageUsageFlagBits>( swapchianUsageFlags & flag);
	}

}

Swapchain::Swapchain(Device& device, VkSurfaceKHR surface, VkExtent2D extent, const std::set<VkImageUsageFlagBits>& imageUsageFlags)
	:device(device),surface(surface),imageUsageFlags(imageUsageFlags)
{
	compImageUsageFlags();
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport();

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);

	VkPresentModeKHR presentMode = choosePresentModeKHR(swapChainSupport.presentModes);

	VkExtent2D surfaceExtent = chooseSwapExtent(swapChainSupport.capabilities,extent);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(device.getPhysicalDevice().getHandle(), surfaceFormat.format, &formatProperties);

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageExtent = surfaceExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (device.getGraphicQueue().getFamilyIndex() != device.getPresentQueue().getFamilyIndex())
	{
		throw Error("GraphicQueue need same familyindex with PresentQueue!");
	}
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device.getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
		throw Error("failed to create swap chain!");
	}
	Log("Swapchain created!");

	vkGetSwapchainImagesKHR(device.getHandle(), handle, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(device.getHandle(), handle, &imageCount, images.data());

	imageFormat = surfaceFormat.format;
	swapChainExtent = surfaceExtent;


}
Swapchain::~Swapchain()
{
	if (handle != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(device.getHandle(), handle, nullptr);
	}
}
SwapChainSupportDetails Swapchain::querySwapChainSupport()
{
	SwapChainSupportDetails details;
	const auto gpu = device.getPhysicalDevice().getHandle();

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}
VkPresentModeKHR Swapchain::choosePresentModeKHR(const std::vector<VkPresentModeKHR>& availableModes)
{
	for (const auto& preferMode : presentModePriorityList) {
		
		for (const auto& nowMode : availableModes)
		{
			if (preferMode == nowMode)
			{
				return preferMode;
			}
		}
	}

	Log("Preper present mode can not use!", ERROR_TYPE);
	return availableModes[0];
}
VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& preferFormat : surfaceFormatPriorityList)
	{
		for (const auto& nowFormat : availableFormats)
		{
			if (preferFormat.colorSpace == nowFormat.colorSpace && preferFormat.format == nowFormat.format)
			{
				return preferFormat;
			}
		}
	}
	Log("Preper surface format can not use!", ERROR_TYPE);
	return availableFormats[0];
}
VkExtent2D Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,  VkExtent2D extent)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {

		VkExtent2D actualExtent = extent;

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
			capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height);

		return actualExtent;
		
	}

}
std::vector<VkImage>& Swapchain::getImages()
{
	return images;
}
VkFormat Swapchain::getImageFormat()
{
	return imageFormat;
}
VkExtent2D Swapchain::getExtent()
{
	return swapChainExtent;
}

VkImageUsageFlagBits Swapchain::getImageUsage()
{
	return swapchianUsageFlags;
}

}