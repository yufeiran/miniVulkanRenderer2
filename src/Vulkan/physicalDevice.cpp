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

}
