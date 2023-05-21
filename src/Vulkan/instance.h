#pragma once 

#include"vk_common.h"

namespace mini
{
class PhysicalDevice;

class Instance
{
public:
	Instance(const std::vector<const char*>& requiredExtensions = {},
		const std::vector<const char*>& requiredValidationLayers = { "VK_LAYER_KHRONOS_validation" },
		bool enableValidationLayers = true);
	~Instance();

	void queryGpus();

	// Tries to find the first available discrete GPU that can render to the given surface 
	PhysicalDevice& getSuitableGpu(VkSurfaceKHR);

	// Tries to find the first available discrete GPU
	PhysicalDevice& getFirstGpu();

private:
	VkInstance handle{ VK_NULL_HANDLE };
	std::vector<const char*> enableExtensions;
	VkDebugUtilsMessengerEXT debugUtilsMessenger{ VK_NULL_HANDLE };

	std::vector<std::unique_ptr< PhysicalDevice>> gpus;
};
} // namespace mini