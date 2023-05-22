#pragma once
#include"vk_common.h"
#include"physicalDevice.h"
// 
namespace mini
{
class PhysicalDevice;

class Device
{
public:
	Device(PhysicalDevice& gpu,
		std::vector<const char*> requestdExtensions = {});
	~Device();

	bool isExtensionSupported(const std::string& extension);

private:
	VkDevice handle{ VK_NULL_HANDLE };

	VkSurfaceKHR surface{ VK_NULL_HANDLE };
	
	const PhysicalDevice& gpu;

	std::vector<VkExtensionProperties> supportDeviceExtension;

	std::vector<const char*>enabledExtension{};

	
};

}