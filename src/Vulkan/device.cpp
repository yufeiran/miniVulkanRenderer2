#include "device.h"
#include"commandPool.h"
#include"commandBuffer.h"

namespace mini
{
Device::Device(PhysicalDevice& gpu, 
	VkSurfaceKHR surface,
	std::vector<const char*> requestdExtensions)
	:gpu(gpu),
	surface(surface)
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

	//// Display supported extensions
	//if (supportDeviceExtension.size() > 0)
	//{
	//	Log("Device supports extension:");
	//	for (const auto& extension : supportDeviceExtension)
	//	{
	//		Log(" " + std::string( extension.extensionName));
	//	}
	//}

	std::vector<const char*> unsupportedExtensions{};
	for (auto& extension : requestdExtensions)
	{
		if (isExtensionSupported(extension))
		{
			enabledExtensions.push_back(extension);
		}
		else {
			unsupportedExtensions.push_back(extension);
		}
	}

	if (enabledExtensions.size() > 0)
	{
		Log("Device enable the following extension:");
		for (const auto& it : enabledExtensions)
		{
			Log(it);
		}
	}
	if (unsupportedExtensions.size() > 0)
	{
		Log("Following Device extension not support:", WARNING_TYPE);
		for (const auto& it : unsupportedExtensions)
		{
			Log(it);
		}
	}

	//=======================================================================================

	VkPhysicalDeviceVulkan12Features features12{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
	features12.bufferDeviceAddress                       = VK_TRUE;
	features12.hostQueryReset                            = VK_TRUE;
	features12.runtimeDescriptorArray                    = VK_TRUE;
	features12.scalarBlockLayout                         = VK_TRUE;
	features12.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;


	
	VkPhysicalDeviceAccelerationStructureFeaturesKHR asFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR};
	asFeatures.accelerationStructure=VK_TRUE;
	features12.pNext=&asFeatures;



	VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

	// get all feature request!
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = queueCreateInfos.size();
	createInfo.enabledExtensionCount = enabledExtensions.size();
	createInfo.ppEnabledExtensionNames = enabledExtensions.data();
	createInfo.pEnabledFeatures = &gpu.getFeatures();
	createInfo.pNext = &features12;

	VkResult result = vkCreateDevice(gpu.getHandle(), &createInfo, nullptr, &handle);

	if (result != VK_SUCCESS)
	{
		throw Error("Cannot create device");
	}
	
	// find a queue can do graphic 

	for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyPropertiesCount; ++queueFamilyIndex)
	{
		const VkQueueFamilyProperties& queueFamilyProperty = gpu.getQueueFamilyProperties()[queueFamilyIndex];
		if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			// TODO: check surface support
			VkBool32 presentSupport = gpu.isPresentSupported(surface, queueFamilyIndex);
			int index = 0;
			graphicQueue = std::make_unique<Queue>(*this, queueFamilyIndex, queueFamilyProperty, presentSupport, index);
			Log("Graphic Queue created! queueFamilyIndex:" + toString(queueFamilyIndex)+" index:"+toString(index));
			break;
		}
	}

	
	// find a queue can do present
	for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyPropertiesCount; ++queueFamilyIndex)
	{
		const VkQueueFamilyProperties& queueFamilyProperty = gpu.getQueueFamilyProperties()[queueFamilyIndex];
		if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{

			VkBool32 presentSupport = gpu.isPresentSupported(surface, queueFamilyIndex);
			if (presentSupport)
			{
				int index = 0;
				presentQueue = std::make_unique<Queue>(*this, queueFamilyIndex, queueFamilyProperty, presentSupport, index);
				Log("Present Queue created! queueFamilyIndex:"+toString(queueFamilyIndex) + " index:" + toString(index));
				break;
			}
		}
	}

	commandPoolForTransfer = std::make_unique<CommandPool>(*this,VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
}

Device::~Device()
{
	commandPoolForTransfer.reset();
	if (handle != VK_NULL_HANDLE)
	{
		vkDestroyDevice(handle, nullptr);
	}
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

const PhysicalDevice& Device::getPhysicalDevice() const
{
	return gpu;
}

VkResult Device::waitIdle() const
{
	return vkDeviceWaitIdle(handle);
}

Queue& Device::getGraphicQueue() const
{
	return *graphicQueue;
}

Queue& Device::getPresentQueue() const
{
	return *presentQueue;
}

void Device::copyBuffer(Buffer& srcBuffer, Buffer& dstBuffer, VkDeviceSize size)
{
	auto &cmd= commandPoolForTransfer->createCommandBuffer();

	cmd->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	cmd->copy(srcBuffer, dstBuffer, size);

	cmd->end();

	auto cmdHandle = cmd->getHandle();

	VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdHandle;

	vkQueueSubmit(graphicQueue->getHandle(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicQueue->getHandle());

	vkFreeCommandBuffers(handle, commandPoolForTransfer->getHandle(), 1, &cmdHandle);
}

CommandPool& Device::getCommandPoolForTransfer()
{
	return *commandPoolForTransfer;
}

}

