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

	enum ImageType { SWAPCHAIN_IMG, CREATED_IMG };
	class Image
	{
	public:
		Image(Device& device, VkImage handle, const VkExtent2D& extent, VkFormat format,
			VkImageUsageFlags imageUsage, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

		Image(Device& device, const VkExtent2D& extent, VkFormat format,
			VkImageUsageFlags imageUsage, int layerCount = 1, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

		Image(Device& device, const VkExtent2D& extent, size_t size, const void* data, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, bool needMipmap = true);

		Image(Device& device, const std::string& filename = "../assets/images/yamato.jpg", bool flipTexture = false, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, bool needMipmap = true);

		// for cubemap
		Image(Device& device, const std::vector<std::string>& filenames, bool flipTexture = false, bool needMipmap = true);

		Image(Image&& other);

		~Image();

		VkImage getHandle() const;

		Device& getDevice() const;

		VkFormat getFormat() const;

		VkExtent2D getExtent() const;

		VkImageUsageFlags getUsage() const;

		VkSampleCountFlagBits getSampleCount() const;

		inline int getLayers() const { return layers; }

		uint32_t getMipLevels() const { return mipLevels; }

		void setName(const char* name);


	private:
		void generateMipmaps(VkImage image,uint32_t texWidth, uint32_t texHeight, uint32_t mipLevels,uint32_t layerCount = 1);

		void bindImageMemory(DeviceMemory& deviceMemory);

		void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount = 1, uint32_t mipLevels = 1);

		void copyBufferToImage(Buffer& buffer, int layerCount = 1);

		Device& device;

		ImageType imageType;

		int layers{ 1 };

		uint32_t mipLevels{ 1 };

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
