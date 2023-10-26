#pragma once

#include "Common/common.h"

#include "Vulkan/image.h"
#include "Vulkan/imageView.h"
#include "Vulkan/sampler.h"

namespace mini
{

// only use for easy use , no memory manage
struct Texture
{
	VkImage     image;
	VkImageView imageView;
	VkSampler   sampler;
};



}