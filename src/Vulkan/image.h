#include"vk_common.h"

namespace mini
{
class Device;

/* Image Ҫô�Ǵ�swapchain��Ū�����ģ�Ҫô���Լ�����������
*  ������Ҫ���ִ����������ֱ���������swapchain�����õ�Image���Լ�������
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
