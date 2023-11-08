#pragma once
#include"Common/common.h"



namespace mini
{
class Device;
class Buffer;
class DeviceMemory;

/* Image 要么是从swapchain里弄出来的，要么是自己创建出来的
*  所以需要两种创建函数，分别用来接受swapchain创建好的Image和自己创建的
*/

enum ImageType{SWAPCHAIN_IMG,CREATED_IMG};
class Image
{
public:
	Image(Device& device, VkImage handle, const VkExtent2D& extent, VkFormat format,
		VkImageUsageFlags imageUsage,VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

	Image(Device& device, const VkExtent2D& extent, VkFormat format,
		VkImageUsageFlags imageUsage, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

	Image(Device& device, const VkExtent2D& extent, size_t size, const void* data, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB);

	Image(Device& device, const std::string& filename="../assets/images/yamato.jpg", bool flipTexture = false);

	// for cubemap
	Image(Device& device, const std::vector<std::string>& filenames,bool flipTexture = false);

	Image(Image&& other);

	~Image();

	VkImage getHandle() const;

	Device& getDevice() const;

	VkFormat getFormat() const;

	VkExtent2D getExtent() const;

	VkImageUsageFlags getUsage() const;

	VkSampleCountFlagBits getSampleCount() const;

	
private:


	void bindImageMemory(DeviceMemory& deviceMemory);

	void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, int layerCount = 1);

	void copyBufferToImage(Buffer& buffer, int layerCount = 1);
	
	Device& device;

	ImageType imageType;

	VkImage handle{ VK_NULL_HANDLE };

	std::unique_ptr<DeviceMemory> deviceMemory;

	VkExtent2D extent{};

	VkFormat format{};

	VkImageUsageFlags usage{};

	VkSampleCountFlagBits sampleCount{};

	VkImageTiling tiling{};

	VkImageSubresource subresource{};

	std::string name;


};
}
