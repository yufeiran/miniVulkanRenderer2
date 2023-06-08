#pragma once
#include"Vulkan/vk_common.h"

namespace mini
{
class Buffer;
class Device;
class Model;
class Image;

class ResourceManagement
{
public:
	ResourceManagement(Device&device);
	~ResourceManagement();

	Model& getModelByName(const std::string &name);

	Model& getModelById(uint32_t id);

	Image& getImageByName(const std::string &name);

private:
	Device& device;

	std::map<std::string, std::unique_ptr<Model>> modelMap;
	std::map<std::string, std::unique_ptr<Image>> imageMap;
	uint32_t nowModelIndex ;
};


}

