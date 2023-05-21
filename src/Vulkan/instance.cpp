#include "instance.h"



VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
) {
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {

	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		Log("validation layer : " + toString(pCallbackData->messageIdNumber) + " - " + pCallbackData->pMessageIdName + " : " + pCallbackData->pMessage,WARNING);
	}
	else if (messageSeverity& VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT){
		Log("validation layer : " + toString(pCallbackData->messageIdNumber) + " - " + pCallbackData->pMessageIdName + " : " + pCallbackData->pMessage,ERROR);

	}
	return VK_FALSE;
}

bool enableExtension(const char* requiredExtName,
	const std::vector<VkExtensionProperties>& availableExts,
	std::vector<const char*>& enabledExtensions)
{
	for (auto& availExtIt : availableExts)
	{
		auto it = std::find_if(enabledExtensions.begin(), enabledExtensions.end(),
			[requiredExtName](const char* enabledExtName) {
				return strcmp(enabledExtName, requiredExtName) == 0;
			});
		if ((it != enabledExtensions.end())) {
			// Extension has been enabled 
		}
		else {
			Log("Extension " + std::string(requiredExtName) + " found, enabling it");
			enabledExtensions.emplace_back(requiredExtName);
		}
		return true;
	}
	Log("Extension" + std::string(requiredExtName) + " not found", WARNING);
	return false;
}

bool enableAllExtensions(const std::vector<const char*> requiredExtNames,
	const std::vector<VkExtensionProperties>& availableExts,
	std::vector<const char*>& enabledExtensions)
{
	bool result = true;
	for (const auto& it : requiredExtNames)
	{
		if (enableExtension(it, availableExts, enabledExtensions) == false) {
			result = false;
		}
	}
	return result;
}

bool enableAllValidateLayers(const std::vector<const char*>& required,
	const std::vector<VkLayerProperties>& available)
{
	for (auto layer : required)
	{
		bool found = false;
		for (auto& availableLayer : available)
		{
			if (strcmp(layer, availableLayer.layerName) == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			Log("Validation Layer " + std::string(layer) + " no found", WARNING);
			return false;
		}
	}
	return true;
}

Instance::Instance(const std::vector<const char*>& requiredExtensions,
	const std::vector<const char*>& requiredValidationLayers,
	bool enableValidationLayers )
{

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "miniVulkanRenderer2";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;


	//===================================== enable instance extension ========================================
	uint32_t instanceExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
	std::vector<VkExtensionProperties>	avaliableInstanceExtensions(instanceExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, avaliableInstanceExtensions.data());

	// enable glfw extesion
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (int i = 0; i < glfwExtensionCount; i++)
	{
		enableExtension(glfwExtensions[i], avaliableInstanceExtensions, enableExtensions);
	}

	// enable VK_EXT_debug_utils 
	bool hasDebugUtils = enableExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, avaliableInstanceExtensions, enableExtensions);



	// enable require extension
	enableAllExtensions(requiredExtensions, avaliableInstanceExtensions, enableExtensions);

	//===========================================================================================================

	//===================================== enable validation layer  ========================================
	uint32_t instanceLayerCount;
	vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

	std::vector<VkLayerProperties> availableInstanceLayers(instanceLayerCount);
	vkEnumerateInstanceLayerProperties(&instanceLayerCount, availableInstanceLayers.data());

	if (enableAllValidateLayers(requiredValidationLayers, availableInstanceLayers))
	{
		Log("Enable Validation Layers:");
		for (const auto& layer : requiredValidationLayers)
		{
			Log(layer);
		}
	}
	else
	{
		throw Error("Required validation layers are missing.");
	}



	//========================================================================================================

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = enableExtensions.size();
	createInfo.ppEnabledExtensionNames = enableExtensions.data();
	VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = requiredValidationLayers.size();
		createInfo.ppEnabledLayerNames = requiredValidationLayers.data();
		if (hasDebugUtils) {
			debugUtilsCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugUtilsCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugUtilsCreateInfo.pfnUserCallback = debugCallback;
			debugUtilsCreateInfo.pUserData = nullptr;


			createInfo.pNext = &debugUtilsCreateInfo;
		}
	}
	else {
		createInfo.enabledLayerCount = 0;
	}


	VkResult result = vkCreateInstance(&createInfo, nullptr, &handle);
	if (result != VK_SUCCESS)
	{
		throw Error("Failed to create instance!");
	}
	volkLoadInstance(handle);

	if (hasDebugUtils) {
		auto result = vkCreateDebugUtilsMessengerEXT(handle, &debugUtilsCreateInfo, nullptr, &debugUtilsMessenger);
		if (result != VK_SUCCESS) {
			throw Error("Could not create debug utils messenger");
		}
	}


	Log("Instance created");

}

Instance::~Instance()
{
	if (debugUtilsMessenger != VK_NULL_HANDLE)
	{
		vkDestroyDebugUtilsMessengerEXT(handle, debugUtilsMessenger, nullptr);
	}
	if (handle != VK_NULL_HANDLE) {
		vkDestroyInstance(handle, nullptr);
		Log("Instance destroyed!");
	}

}
