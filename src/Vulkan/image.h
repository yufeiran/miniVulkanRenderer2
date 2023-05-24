#include"vk_common.h"

namespace mini
{
class Device;
class Image
{
public:
	Image( VkImage handle, Device& device, const VkExtent2D& extent, VkFormat format);
	VkImage getHandle() const;

	Device& getDevice() const;
private:
	Device& device;

	VkImage handle{ VK_NULL_HANDLE };

	VkExtent2D extent{};

	VkFormat format{};

};
}
