#pragma once 

#include"Common/common.h"

namespace mini
{
class Device;
class Image;
class ImageView;

class Sampler
{
public:
	Sampler(Device& device);
	~Sampler();

	VkSampler getHandle();
private:
	VkSampler handle{ VK_NULL_HANDLE };
	Device& device;


};

}