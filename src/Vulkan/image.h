#pragma once
#include"vk_common.h"



namespace mini
{
class Device;
class Buffer;
class DeviceMemory;

/* Image Ҫô�Ǵ�swapchain��Ū�����ģ�Ҫô���Լ�����������
*  ������Ҫ���ִ����������ֱ���������swapchain�����õ�Image���Լ�������
*/

enum ImageType{SWAPCHAIN_IMG,CREATED_IMG};
class Image
{
public:
	Image(Device& device, VkImage handle, const VkExtent2D& extent, VkFormat format,
		VkImageUsageFlags imageUsage,VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

	Image(Device& device, const VkExtent2D& extent, VkFormat format,
		VkImageUsageFlags imageUsage, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

	Image(Device& device, const std::string& filename="../assets/images/yamato.jpg");

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

	void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

	void copyBufferToImage(Buffer& buffer);
	
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
