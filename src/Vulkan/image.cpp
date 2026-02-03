#include"image.h"
#include"device.h"
#include"buffer.h"
#include"deviceMemory.h"
#include<stb_image.h>
#include"commandBuffer.h"
#include"commandPool.h"

namespace mini
{



	Image::Image(Device& device, VkImage handle, const VkExtent2D& extent, VkFormat format,
		VkImageUsageFlags imageUsage, VkSampleCountFlagBits sampleCount) :
		device(device), handle(handle), extent(extent), format(format), usage(imageUsage), sampleCount(sampleCount), imageType(SWAPCHAIN_IMG)
	{

	}

	Image::Image(Device& device, const VkExtent2D& extent, VkFormat format, VkImageUsageFlags imageUsage, int layerCount, VkSampleCountFlagBits sampleCount)
		:device(device), extent(extent), format(format), usage(imageUsage), sampleCount(sampleCount), imageType(CREATED_IMG)

	{
		VkImageCreateInfo imageInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = extent.width;
		imageInfo.extent.height = extent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = layerCount;
		this->layers = layerCount;

		imageInfo.format = format;
		format = imageInfo.format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = imageUsage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		imageInfo.samples = sampleCount;
		imageInfo.flags = 0;

		if (layerCount == 6)
		{
			imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		}

		if (vkCreateImage(device.getHandle(), &imageInfo, nullptr, &handle) != VK_SUCCESS) {
			throw Error("Failed to create image");
		}
		deviceMemory = std::make_unique<DeviceMemory>(device, *this, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		bindImageMemory(*deviceMemory);

		if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, layerCount);
		}
		else
		{
			transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, layerCount);
		}
	}

	Image::Image(Device& device, const VkExtent2D& extent, size_t size, const void* data, VkFormat format, bool needMipmap)
		:device(device), extent(extent), format(format), imageType(CREATED_IMG)
	{

		// Staging buffer
		Buffer stagingBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer.map(data, size);

		uint32_t mipLevelsCount = 1;
		if (needMipmap == true)
		{
			mipLevelsCount = static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, extent.height)))) + 1;
		}
		this->mipLevels = mipLevelsCount;
		this->layers = 1;

		VkImageCreateInfo imageInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = extent.width;
		imageInfo.extent.height = extent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevelsCount;
		imageInfo.arrayLayers = 1;

		imageInfo.format = format;
		format = imageInfo.format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;
		if (vkCreateImage(device.getHandle(), &imageInfo, nullptr, &handle) != VK_SUCCESS) {
			throw Error("Failed to create image");
		}

		deviceMemory = std::make_unique<DeviceMemory>(device, *this, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		bindImageMemory(*deviceMemory);

		transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, this->layers, this->mipLevels);

		copyBufferToImage(stagingBuffer);

		if (needMipmap == true)
		{
			generateMipmaps(handle, extent.width, extent.height, mipLevelsCount);
		}
		else {
			transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->layers, this->mipLevels);
		}

	}

	Image::Image(Device& device, const std::string& filename, bool flipTexture, VkFormat inFormat, bool needMipmap) :device(device), name(filename), imageType(CREATED_IMG)
	{
		int texWidth, texHeight, texChannels;

		stbi_set_flip_vertically_on_load(flipTexture);


		stbi_uc* pixels = stbi_load(name.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		extent.width = texWidth;
		extent.height = texHeight;
		VkDeviceSize imageSize = extent.width * extent.height * 4;

		if (!pixels) {
			throw Error("Failed to load texture image:" + filename);
		}

		// Staging buffer
		Buffer stagingBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer.map(pixels, imageSize);


		stbi_image_free(pixels);

		uint32_t mipLevels = 1;
		if (needMipmap)
		{
			mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
		}
		this->mipLevels = mipLevels;
		this->layers = 1;

		VkImageCreateInfo imageInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = extent.width;
		imageInfo.extent.height = extent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;

		imageInfo.format = inFormat;
		format = imageInfo.format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;
		if (vkCreateImage(device.getHandle(), &imageInfo, nullptr, &handle) != VK_SUCCESS) {
			throw Error("Failed to create image");
		}

		deviceMemory = std::make_unique<DeviceMemory>(device, *this, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		bindImageMemory(*deviceMemory);

		transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layers, mipLevels);

		copyBufferToImage(stagingBuffer);

		if (needMipmap == true)
		{
			generateMipmaps(handle, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), mipLevels);
		}
		else
		{
			transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, layers, mipLevels);
		}

	}


	// for create cubemap
	Image::Image(Device& device, const std::vector<std::string>& filenames, bool flipTexture, bool needMipmap) :device(device), name(filenames[0]), imageType(CREATED_IMG)
	{

		// only handle cubemap
		assert(filenames.size() == 6);
		this->layers = 6;

		// Load Textures
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels[6] = {};

		for (int i = 0; i < 6; i++)
		{

			stbi_set_flip_vertically_on_load(flipTexture);


			pixels[i] = stbi_load(filenames[i].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			extent.width = texWidth;
			extent.height = texHeight;


			if (!pixels) {
				throw Error("Failed to load texture image:" + filenames[i]);
			}
		}

		const VkDeviceSize imageSize = extent.width * extent.height * 4 * 6;
		const VkDeviceSize layerSize = imageSize / 6;

		uint32_t mipLevels = 1;
		if (needMipmap)
		{
			mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
		}
		this->mipLevels = mipLevels;

		// Staging buffer
		Buffer stagingBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


		stagingBuffer.persistentMap(imageSize);

		void* mapAddress = stagingBuffer.getMapAddress();


		for (int i = 0; i < 6; i++)
		{
			memcpy((char*)mapAddress + layerSize * i, pixels[i], (size_t)layerSize);
		}

		stagingBuffer.unpersistentMap();


		for (int i = 0; i < 6; i++)
		{
			stbi_image_free(pixels[i]);
		}


		VkImageCreateInfo imageInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = extent.width;
		imageInfo.extent.height = extent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 6;

		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		format = imageInfo.format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

		if (vkCreateImage(device.getHandle(), &imageInfo, nullptr, &handle) != VK_SUCCESS) {
			throw Error("Failed to create image");
		}

		deviceMemory = std::make_unique<DeviceMemory>(device, *this, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		bindImageMemory(*deviceMemory);

		transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 6,mipLevels);

		copyBufferToImage(stagingBuffer, 6);

		if (needMipmap == true)
		{
			generateMipmaps(handle, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), mipLevels, 6);
		}
		else {
			transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 6, mipLevels);
		}



	}

	Image::Image(Image&& other) :device(other.device), imageType(other.imageType), handle(other.handle),
		deviceMemory(std::move(other.deviceMemory)), extent(other.extent), format(other.format),
		usage(other.usage), sampleCount(other.sampleCount), tiling(other.tiling), subresource(other.subresource), name(other.name), layers(other.layers)

	{
		other.handle = VK_NULL_HANDLE;
		other.deviceMemory = nullptr;
	}

	Image::~Image()
	{
		if (imageType == CREATED_IMG) {
			if (handle != VK_NULL_HANDLE)
			{
				vkDestroyImage(device.getHandle(), handle, nullptr);

			}
		}
	}

	VkImage Image::getHandle() const
	{
		return handle;
	}

	Device& Image::getDevice() const
	{
		return device;
	}

	VkFormat Image::getFormat() const
	{
		return format;
	}

	VkExtent2D Image::getExtent() const
	{
		return extent;
	}

	VkImageUsageFlags Image::getUsage() const
	{
		return usage;
	}

	VkSampleCountFlagBits Image::getSampleCount() const
	{
		return sampleCount;
	}

	void Image::generateMipmaps(VkImage image, uint32_t texWidth, uint32_t texHeight, uint32_t mipLevels, uint32_t layers)
	{
		auto cmd = device.getCommandPoolForTransfer().createCommandBuffer();
		cmd->beginSingleTime();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layers;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = texWidth;
		int32_t mipHeight = texHeight;

		for (uint32_t i = 1; i < mipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(cmd->getHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { static_cast<int32_t>(mipWidth), static_cast<int32_t>(mipHeight), 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = layers;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1,1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = layers;

			vkCmdBlitImage(cmd->getHandle(),
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(cmd->getHandle(),
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1)mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmd->getHandle(),
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		cmd->endSingleTime(device.getGraphicQueue());

	}

	void Image::bindImageMemory(DeviceMemory& deviceMemory)
	{
		vkBindImageMemory(device.getHandle(), handle, deviceMemory.getHandle(), 0);
	}

	void Image::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount, uint32_t mipLevels)
	{
		auto cmd = device.getCommandPoolForTransfer().createCommandBuffer();
		cmd->beginSingleTime();

		VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = handle;
		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (device.getPhysicalDevice().hasStencilComponent(format)) {
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layerCount;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL)
		{
			// may have bug
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else {
			throw Error("unsupported layout transition!");
		}


		vkCmdPipelineBarrier(
			cmd->getHandle(),
			sourceStage,
			destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		cmd->endSingleTime(device.getGraphicQueue());
	}

	void Image::copyBufferToImage(Buffer& buffer, int layerCount)
	{
		auto cmd = device.getCommandPoolForTransfer().createCommandBuffer();

		cmd->beginSingleTime();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;

		region.imageOffset = { 0,0,0 };
		region.imageExtent = {
			extent.width,
			extent.height,
			1
		};

		vkCmdCopyBufferToImage(
			cmd->getHandle(),
			buffer.getHandle(),
			handle,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		cmd->endSingleTime(device.getGraphicQueue());
	}

}