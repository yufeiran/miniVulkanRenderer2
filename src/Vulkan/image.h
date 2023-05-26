#include"vk_common.h"

namespace mini
{
class Device;

/* Image 要么是从swapchain里弄出来的，要么是自己创建出来的
*  所以需要两种创建函数，分别用来接受swapchain创建好的Image和自己创建的
*/
class Image
{
public:
	Image( VkImage handle, Device& device, const VkExtent2D& extent, VkFormat format,
		VkImageUsageFlagBits imageUsage,VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
	VkImage getHandle() const;

	Device& getDevice() const;

	VkFormat getFormat() const;

	VkExtent2D getExtent() const;

	VkImageUsageFlags getUsage() const;

	VkSampleCountFlagBits getSampleCount() const;

	
private:
	Device& device;

	VkImage handle{ VK_NULL_HANDLE };

	VkExtent2D extent{};

	VkFormat format{};

	VkImageUsageFlags usage{};

	VkSampleCountFlagBits sampleCount{};

	VkImageTiling tiling{};

	VkImageSubresource subresource{};


};
}
