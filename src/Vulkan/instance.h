#pragma once 

#include"Common/common.h"

namespace mini
{
class PhysicalDevice;

class Instance
{
public:
	Instance(const std::vector<const char*>& requiredExtensions = {},
		const std::vector<const char*>& requiredValidationLayers = { "VK_LAYER_KHRONOS_validation","VK_LAYER_LUNARG_monitor" },
		bool enableValidationLayers = true);
	~Instance();

	void queryGpus();

	// Tries to find the first available discrete GPU that can render to the given surface 
	PhysicalDevice& getSuitableGpu(VkSurfaceKHR);

	// Tries to find the first available discrete GPU
	PhysicalDevice& getFirstGpu();

	bool isEnable(const char* extension)const;

	VkInstance getHandle() const;

private:
	VkInstance handle{ VK_NULL_HANDLE };
	std::vector<const char*> enableExtensions;
	VkDebugUtilsMessengerEXT debugUtilsMessenger{ VK_NULL_HANDLE };

	std::vector<std::shared_ptr< PhysicalDevice>> gpus;
};
} // namespace mini