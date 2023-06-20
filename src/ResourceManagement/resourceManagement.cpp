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
		modelMap["triangle"] = std::make_unique<Model>(device,"../../assets/BattleCruiser/BattleCruiser.obj", "triangle", nowModelIndex++);
		imageMap["yamato"] = std::make_unique<Image>(device, "../../assets/BattleCruiser/battlecruiser_diffuse.png");
		imageViewMap["yamato"] = std::make_unique<ImageView>(*imageMap["yamato"]);

		defaultSampler = std::make_unique<Sampler>(device);
	}

	ResourceManagement::~ResourceManagement()
	{

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

}