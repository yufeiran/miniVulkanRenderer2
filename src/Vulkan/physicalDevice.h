#pragma once 
#include"vk_common.h"

namespace mini
{

	class Instance;

	/*
	*  @brief A wrapper class for VkPhysicalDeivce
	*  handling gpu features, properties, and queue families for the device creation
	*/
	class PhysicalDevice
	{
	public:
		PhysicalDevice(Instance& instance, VkPhysicalDevice physicalDevice);

		const VkPhysicalDeviceFeatures& getFeatures() const;

		const VkPhysicalDeviceProperties& getProperties() const;

		const VkPhysicalDeviceMemoryProperties& getMemoryProperties() const;

		const std::vector<VkQueueFamilyProperties>& getQueueFamilyProperties() const;

		VkBool32 isPresentSupported(VkSurfaceKHR surface, uint32_t queueFamilyIndex)const;

		VkPhysicalDevice getHandle() const;

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)const;

		VkFormat findDepthFormat()const;

		bool hasStencilComponent(VkFormat format) const; 

	template<typename T>
	T& requestExtensionFeatures(VkStructureType type)
	{
		// TODO: support Add features!
	}


	private:
		Instance& instance;

		VkPhysicalDevice handle{ VK_NULL_HANDLE };

		// The features that GPU supports
		VkPhysicalDeviceFeatures features{};

		// The GPU properties
		VkPhysicalDeviceProperties properties;

		// The GPU memory properties
		VkPhysicalDeviceMemoryProperties memoryProperties;

		// The GPU queue family properties 
		std::vector<VkQueueFamilyProperties> queueFamilyProperties;

		// The features that need to be enabled in the logical device
		VkPhysicalDeviceFeatures requestedFeatures{};


	};
}
