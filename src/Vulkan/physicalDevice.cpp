#include "physicalDevice.h"


namespace mini
{
PhysicalDevice::PhysicalDevice(Instance& instance, VkPhysicalDevice physicalDevice):
	instance{ instance },
	handle{physicalDevice}
{
	vkGetPhysicalDeviceFeatures(physicalDevice, &features);
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	Log("Found GPU: " + std::string(properties.deviceName));
	uint32_t queueFamilyPropertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, nullptr);
	queueFamilyProperties = std::vector<VkQueueFamilyProperties>(queueFamilyPropertiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

}

const VkPhysicalDeviceFeatures& PhysicalDevice::getFeatures() const
{
	return features;
}

const VkPhysicalDeviceProperties& PhysicalDevice::getProperties() const
{
	return properties;
}

const VkPhysicalDeviceMemoryProperties& PhysicalDevice::getMemoryProperties() const
{
	return memoryProperties;
}

const std::vector<VkQueueFamilyProperties>& PhysicalDevice::getQueueFamilyProperties() const
{

	return queueFamilyProperties;
}

VkBool32 PhysicalDevice::isPresentSupported(VkSurfaceKHR surface, uint32_t queueFamilyIndex) const
{
	VkBool32 presentSupported{ VK_FALSE };

	if (surface != VK_NULL_HANDLE)
	{
		VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(handle, queueFamilyIndex, surface, &presentSupported));
	}

	return presentSupported;
}

VkPhysicalDevice PhysicalDevice::getHandle() const
{
	return handle;
}

VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(handle, format, &props);
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}

	}
	
	throw Error("Failed to find supported format!");
}

VkFormat PhysicalDevice::findDepthFormat() const 
{
	return findSupportedFormat(
		{VK_FORMAT_D32_SFLOAT,VK_FORMAT_D32_SFLOAT_S8_UINT,VK_FORMAT_D24_UNORM_S8_UINT},
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool PhysicalDevice::hasStencilComponent(VkFormat format) const
{
	return format==VK_FORMAT_D32_SFLOAT_S8_UINT||format==VK_FORMAT_D24_UNORM_S8_UINT;
}

}
