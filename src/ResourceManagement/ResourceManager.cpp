#include "ResourceManagement/ResourceManager.h"
#include "Vulkan/device.h"
#include "Vulkan/physicalDevice.h"
#include "Vulkan/image.h"
#include "Vulkan/imageView.h"
#include "Vulkan/sampler.h"
#include "ResourceManagement/objLoader.h"
#include "Vulkan/commandPool.h"
#include "Vulkan/buffer.h"
#include <cmath>

namespace mini
{	


ResourceManager::ResourceManager(Device& device):
	device(device)
{
	cmdPool = std::make_unique<CommandPool>(device);
	defaultSampler = std::make_unique<Sampler>(device);
}

ResourceManager::~ResourceManager()
{
	textures.clear();
	
}

void ResourceManager::loadModel(std::string name, std::string path, glm::mat4 transform, bool flipTexture)
{
	//if(modelMap.find(name)!=modelMap.end())
	//{
	//	return;
	//}
	ObjLoader loader;
	loader.loadModel(path);

	// Converting from Srgb to linear
	for(auto& m : loader.materials)
	{
		m.ambient  = {pow(m.ambient[0],2.2f),pow(m.ambient[1],2.2f),pow(m.ambient[2],2.2f)};
		m.diffuse  = {pow(m.diffuse[0],2.2f),pow(m.diffuse[1],2.2f),pow(m.diffuse[2],2.2f)};
		m.specular = {pow(m.specular[0],2.2f),pow(m.specular[1],2.2f),pow(m.specular[2],2.2f)};
	}

	std::unique_ptr<ObjModel> model = std::make_unique<ObjModel>();
	model->nbIndices  = static_cast<uint32_t>(loader.indices.size());
	model->nbVertices = static_cast<uint32_t>(loader.vertices.size());

	// Create the buffers on Device and copy vertices , indices and materials 
	VkBufferUsageFlags flag           = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	VkBufferUsageFlags rayTracingFlags = 
		flag | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

	model->vertexBuffer   = std::make_unique<Buffer>(device, loader.vertices, static_cast<VkBufferUsageFlagBits>( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | rayTracingFlags));
	model->indexBuffer    = std::make_unique<Buffer>(device, loader.indices, static_cast<VkBufferUsageFlagBits>( VK_BUFFER_USAGE_INDEX_BUFFER_BIT | rayTracingFlags));
	model->matColorBuffer = std::make_unique<Buffer>(device, loader.materials, static_cast<VkBufferUsageFlagBits>( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | flag));
	model->matIndexBuffer = std::make_unique<Buffer>(device, loader.matIndx, static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | flag));

	auto txtOffset = static_cast<uint32_t>(textures.size());
	auto pos=path.find_last_of("/");

	std::string baseDirPath = path.substr(0, pos);
	baseDirPath += "/";
	createTextureImages(loader.textures,baseDirPath,flipTexture);

	ObjInstance instance;
	instance.transform = transform;
	instance.objIndex  = static_cast<uint32_t>(objModel.size());
	instance.name = name;
	instances.push_back(instance);

	ObjDesc desc={};
	desc.txtOffset            = txtOffset;
	desc.vertexAddress        = model->vertexBuffer->getBufferDeviceAddress();
	desc.indexAddress         = model->indexBuffer->getBufferDeviceAddress();
	desc.materialAddress      = model->matColorBuffer->getBufferDeviceAddress();
	desc.materialIndexAddress = model->matIndexBuffer->getBufferDeviceAddress();

	objModel.emplace_back(std::move(model));
	objDesc.emplace_back(std::move(desc));

}

void ResourceManager::loadCubemap(const std::vector<std::string>& cubeMapNames, bool flipTexture)
{
	cubeMap     = std::make_unique<Image>(device, cubeMapNames, flipTexture);
	cubeMapView = std::make_unique<ImageView>(*cubeMap,VK_FORMAT_UNDEFINED,VK_IMAGE_VIEW_TYPE_CUBE);


	cubeMapTexture.image  = cubeMap->getHandle();
	cubeMapTexture.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	cubeMapTexture.descriptor.imageView   = cubeMapView->getHandle();
	cubeMapTexture.descriptor.sampler     = defaultSampler->getHandle();


}

void ResourceManager::createTextureImages(const std::vector<std::string>& texturesStr, const std::string &modelPath, bool flipTexture)
{

	if(texturesStr.empty() && textures.empty())
	{
		std::array<uint8_t,4>       color{255u, 255u, 255u,255u};
		size_t                      imgBufferSize = sizeof(color);
		VkExtent2D                  imgSize       = VkExtent2D{1, 1};
		std::unique_ptr<Image>      image         = std::make_unique<Image>(device, imgSize, imgBufferSize, color.data());
		std::unique_ptr<ImageView>  imageView     = std::make_unique<ImageView>(*image);

		Texture texture;

		texture.image  = image->getHandle();
		texture.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		texture.descriptor.imageView   = imageView->getHandle();
		texture.descriptor.sampler     = defaultSampler->getHandle();

		images.push_back(std::move(image));
		imageViews.push_back(std::move(imageView));
		textures.push_back(texture);
	}
	else
	{
		for(const auto& textureStr : texturesStr)
		{
			std::string imagePath = modelPath + textureStr;
			Log("imagePath"+imagePath);
			std::unique_ptr<Image>     image     = std::make_unique<Image>(device,imagePath,flipTexture);
			std::unique_ptr<ImageView> imageView = std::make_unique<ImageView>(*image);
		
			Texture texture;
			texture.image                  = image->getHandle();
			texture.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			texture.descriptor.imageView   = imageView->getHandle();
			texture.descriptor.sampler     = defaultSampler->getHandle();
		
			images.push_back(std::move(image));
			imageViews.push_back(std::move(imageView));
			textures.push_back(texture);
		}
	}
}

uint32_t ResourceManager::getInstanceId(const std::string name)
{
	int id = 0;
	for(auto & instance: instances)
	{
		if(name == instance.name)
		{
			return id;
		}
		id++;
	}
	throw id;
}

}