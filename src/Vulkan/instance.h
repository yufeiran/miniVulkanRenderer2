#pragma once 

#include"vk_common.h"


class Instance
{
public:
	Instance(const std::vector<const char*>& requiredExtensions = {},
		const std::vector<const char*>&requiredValidationLayers={"VK_LAYER_KHRONOS_validation"},
		bool enableValidationLayers=true);
	~Instance();


private:
	VkInstance handle{ VK_NULL_HANDLE };
	std::vector<const char*> enableExtensions;
	VkDebugUtilsMessengerEXT debugUtilsMessenger{ VK_NULL_HANDLE };

	
};