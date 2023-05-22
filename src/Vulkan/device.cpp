#include "device.h"

namespace mini
{
Device::Device(PhysicalDevice& gpu, std::vector<const char*> requestdExtensions)
	:gpu(gpu)
{
	Log("Selected GPU: " + std::string(gpu.getProperties().deviceName));

	// Perpare the device queue
	uint32_t                             queueFamilyPropertiesCount = gpu.getQueueFamilyProperties().size();
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(queueFamilyPropertiesCount, { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO });
	std::vector<std::vector<float>>      queuePriorities(queueFamilyPropertiesCount);

	for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyPropertiesCount; ++queueFamilyIndex)
	{
		const VkQueueFamilyProperties& queueFamilyProperty = gpu.getQueueFamilyProperties()[queueFamilyIndex];
		queuePriorities[queueFamilyIndex].resize(queueFamilyProperty.queueCount, 1.0f);
		VkDeviceQueueCreateInfo& queueCreateInfo = queueCreateInfos[queueFamilyIndex];

		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreateInfo.queueCount = queueFamilyProperty.queueCount;
		queueCreateInfo.pQueuePriorities = queuePriorities[queueFamilyIndex].data();

	}
	//======================================= Open Device Extension ============================================
	// Check device extension
	uint32_t deviceExtensionCount;
	VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.getHandle(), nullptr, &deviceExtensionCount, nullptr));
	supportDeviceExtension = std::vector<VkExtensionProperties>(deviceExtensionCount);
	VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.getHandle(), nullptr, &deviceExtensionCount, supportDeviceExtension.data()));

	// Display supported extensions
	if (supportDeviceExtension.size() > 0)
	{
		Log("Device supports extension:");
		for (const auto& extension : supportDeviceExtension)
		{
			Log(" " + std::string( extension.extensionName));
		}
	}

	std::vector<const char*> unsupportedExtensions{};
	for (auto& extension : requestdExtensions)
	{
		if (isExtensionSupported(extension))
		{
			enabledExtension.push_back(extension);
		}
		else {
			unsupportedExtensions.push_back(extension);
		}
	}

	if (enabledExtension.size() > 0)
	{
		Log("Device support the following extension:");
		for (const auto& it : enabledExtension)
		{
			Log(it);
		}
	}
	if (unsupportedExtensions.size() > 0)
	{
		Log("Following Device extension not support:", WARNING);
		for (const auto& it : unsupportedExtensions)
		{
			Log(it);
		}
	}

	//=======================================================================================

	VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

	// get all feature request!
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = queueCreateInfos.size();

	createInfo.pEnabledFeatures = &gpu.getFeatures();

	VkResult result = vkCreateDevice(gpu.getHandle(), &createInfo, nullptr, &handle);

	if (result != VK_SUCCESS)
	{
		throw Error("Cannot create device");
	}

}

Device::~Device()
{
}

bool Device::isExtensionSupported(const std::string& extension)
{
	for (const auto& it : supportDeviceExtension)
	{
		if (strcmp(extension.c_str(), it.extensionName) == 0)
		{
			return true;
		}
	}
	return false;
}

}

