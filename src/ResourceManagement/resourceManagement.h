#pragma once
#include"Vulkan/vk_common.h"

namespace mini
{
class Buffer;
class Device;
class Model;
class Image;
class ImageView;
class Sampler;

class ResourceManagement
{
public:
	ResourceManagement(Device&device);
	~ResourceManagement();

	Model& getModelByName(const std::string &name);

	Model& getModelById(uint32_t id);

	Image& getImageByName(const std::string &name);

	ImageView& getImageViewByName(const std::string& name);

	Sampler& getDefaultSampler();

	ImageView& getDefaultImageView();

	std::map<std::string, std::unique_ptr<Model>>& getModelMap();

private:
	Device& device;

	std::map<std::string, std::unique_ptr<Model>> modelMap;
	std::map<std::string, std::unique_ptr<Image>> imageMap;
	std::map<std::string, std::unique_ptr<ImageView>> imageViewMap;
	uint32_t nowModelIndex ;
	std::unique_ptr<Sampler> defaultSampler;
};


}

