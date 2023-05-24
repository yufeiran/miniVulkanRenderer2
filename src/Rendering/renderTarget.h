#include"Vulkan/vk_common.h"

namespace mini
{
class Device;
class Image;

class RenderTarget
{
public:

private:
	Device const& device;

	VkExtent2D extent{};

	std::vector<Image>images;


};
}