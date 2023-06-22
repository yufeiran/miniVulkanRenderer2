#include"resourceManagement.h"
#include"Vulkan/device.h"
#include"Vulkan/physicalDevice.h"
#include"model.h"
#include"Vulkan/image.h"
#include"Vulkan/imageView.h"
#include"Vulkan/sampler.h"

namespace mini
{



	ResourceManagement::ResourceManagement(Device& device):
		device(device), nowModelIndex(0)
	{
		imageMap["defaultTexture"] = std::make_unique<Image>(device, "../../assets/defaultTexture.png");
		imageViewMap["defaultTexture"] = std::make_unique<ImageView>(*imageMap["defaultTexture"]);

		defaultSampler = std::make_unique<Sampler>(device);
	}

	ResourceManagement::~ResourceManagement()
	{

	}

	void ResourceManagement::loadModel( std::string name, std::string path)
	{
		if (modelMap.find(name) == modelMap.end())
		{
			modelMap[name] = std::make_unique<Model>(device, path, name, nowModelIndex++);
		}
		else {
			Log("model name " + name + " is already load", ERROR_TYPE);
		}

	}

	Model& ResourceManagement::getModelByName(const std::string &name)
	{
		if (modelMap.find(name) != modelMap.end()) {
			return *modelMap[name];
		}
		throw Error(("Can't find model by name:" + name).c_str());
	}

	Model& ResourceManagement::getModelById(uint32_t id)
	{
		for (auto& it : modelMap)
		{
			if (it.second->getID() == id)
			{
				return *it.second;
			}
		}
		throw Error(("Can't find model by id:" + toString(id)).c_str());
	}

	Image& ResourceManagement::getImageByName(const std::string& name)
	{
		if (imageMap.find(name) != imageMap.end()) {
			return *imageMap[name];
		}
		throw Error("Can't find image by name:" + name);
	}

	ImageView& ResourceManagement::getImageViewByName(const std::string& name)
	{
		if (imageViewMap.find(name) != imageViewMap.end()) {
			return *imageViewMap[name];
		}
		throw Error("Can't find image by name:" + name);
	}

	Sampler& ResourceManagement::getDefaultSampler()
	{
		return *defaultSampler;
	}

	ImageView& ResourceManagement::getDefaultImageView()
	{
		return *imageViewMap["defaultTexture"];
	}

	std::map<std::string, std::unique_ptr<Model>>& ResourceManagement::getModelMap()
	{
		return modelMap;
	}

}