#pragma once
#include"Vulkan/vk_common.h"

namespace mini
{
class Buffer;
class Device;
class Model;

class ResourceManagement
{
public:
	ResourceManagement(Device&device);
	~ResourceManagement();

	Model& getModelByName(std::string name);

	Model& getModelById(uint32_t id);

private:
	Device& device;

	std::map<std::string, std::unique_ptr<Model>> modelMap;
	uint32_t nowModelIndex ;
};


}

