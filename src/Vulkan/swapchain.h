#pragma once

#include"vk_common.h"

namespace mini
{
class Device;
class ImageView;

enum ImageFormat
{
	sRGB,
	UNORM
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct SwapchainProperties
{
	VkSwapchainKHR oldSwapchain;
	uint32_t imageCount{ 3 };
	VkExtent2D extent{};
	VkSurfaceFormatKHR surfaceFormat{};
	uint32_t arrayLayers;
	VkImageUsageFlags imageUsage;
	VkSurfaceTransformFlagBitsKHR preTransform;
	VkCompositeAlphaFlagBitsKHR compositeAlpha;
	VkPresentModeKHR presentMode;
};

class Swapchain
{
public:
	Swapchain(Device&device , VkSurfaceKHR surface,VkExtent2D extent,
		const std::set<VkImageUsageFlagBits>&imageUsageFlags ={VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,VK_IMAGE_USAGE_TRANSFER_SRC_BIT});

	void create();

	~Swapchain();

	SwapChainSupportDetails querySwapChainSupport();

	VkPresentModeKHR choosePresentModeKHR(const std::vector < VkPresentModeKHR>& availableModes);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,VkExtent2D extent);

	std::vector<VkImage>& getImages();

	VkFormat getImageFormat();

	VkExtent2D getExtent();

	VkImageUsageFlags getImageUsage();

	


private:
	VkImageUsageFlagBits compImageUsageFlags();
	Device& device;

	VkSurfaceKHR surface{ VK_NULL_HANDLE };

	VkSwapchainKHR handle{ VK_NULL_HANDLE };

	std::vector<VkImage> images;

	VkFormat imageFormat;

	VkExtent2D swapChainExtent;

	std::vector<VkSurfaceFormatKHR> surfaceFormats{};

	std::vector<VkPresentModeKHR> presentModes{};

	SwapchainProperties properties;


	// A list for present modes in order of priority ( 0 is highest)
	std::vector<VkPresentModeKHR> presentModePriorityList = {
		VK_PRESENT_MODE_MAILBOX_KHR,
		VK_PRESENT_MODE_FIFO_KHR
	};

	// A list for surface formats perfer choosen
	std::vector<VkSurfaceFormatKHR> surfaceFormatPriorityList = {
		{VK_FORMAT_R8G8B8A8_SRGB,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
		{VK_FORMAT_B8G8R8A8_SRGB,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}
	};

	std::set<VkImageUsageFlagBits> imageUsageFlags;
};

};