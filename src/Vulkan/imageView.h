#include"vk_common.h"

namespace mini
{

	class Image;
	class Device;
	class ImageView
	{
	public:
		ImageView(Image& image, VkFormat format = VK_FORMAT_UNDEFINED, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D);
		~ImageView();



	private:

		Device& device;

		VkImageView handle{ VK_NULL_HANDLE };

		VkFormat format{};

		Image* image{};

	};


}