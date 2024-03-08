#include"sampler.h"
#include"device.h"


namespace mini
{

Sampler::Sampler(Device& device,VkSamplerAddressMode addressMode ,VkSamplerMipmapMode mipmapMode ):device(device)
{
	VkSamplerCreateInfo samplerInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = addressMode;
	samplerInfo.addressModeV = addressMode;
	samplerInfo.addressModeW = addressMode;

	samplerInfo.anisotropyEnable = VK_TRUE;
	VkPhysicalDeviceProperties properties=device.getPhysicalDevice().getProperties();
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = mipmapMode;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 100.0f;

	if (vkCreateSampler(device.getHandle(), &samplerInfo, nullptr, &handle) != VK_SUCCESS) {
		throw Error("Failed to create sampler!");
	}



}

Sampler::Sampler(Device& device, const VkSamplerCreateInfo& createInfo):device(device)
{
	if (vkCreateSampler(device.getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
		throw Error("Failed to create sampler!");
	}

}

Sampler::~Sampler()
{
	if (handle != VK_NULL_HANDLE)
	{
		vkDestroySampler(device.getHandle(), handle, nullptr);
	}
}

VkSampler Sampler::getHandle()
{
	return handle;
}

}