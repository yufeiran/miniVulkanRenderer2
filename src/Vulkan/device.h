#pragma once
#include"vk_common.h"
#include"physicalDevice.h"
#include"queue.h"

// 
namespace mini
{
class PhysicalDevice;

class Device
{
public:
	Device(PhysicalDevice& gpu,
		VkSurfaceKHR surface,
		std::vector<const char*> requestdExtensions = {});
	~Device();

	bool isExtensionSupported(const std::string& extension);

	const VkDevice& getHandle() const
	{
		return handle;
	}

	const PhysicalDevice& getPhysicalDevice() const;

	Queue& getGraphicQueue() const;
	Queue& getPresentQueue() const;

private:
	VkDevice handle{ VK_NULL_HANDLE };

	VkSurfaceKHR surface{ VK_NULL_HANDLE };
	
	const PhysicalDevice& gpu;

	std::vector<VkExtensionProperties> supportDeviceExtension;

	std::vector<const char*>enabledExtensions{};

	std::unique_ptr<Queue>graphicQueue;

	std::unique_ptr<Queue>presentQueue;

};

}