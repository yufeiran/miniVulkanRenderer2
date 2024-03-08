
#include <omp.h>

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
	repeatSampler  = std::make_unique<Sampler>(device,VK_SAMPLER_ADDRESS_MODE_REPEAT);
}

ResourceManager::~ResourceManager()
{
	textures.clear();
	
}

void initMaterial(GltfShadeMaterial& mat)
{
	mat.type = 0;
	mat.ambient        = glm::vec3(0.1f, 0.1f, 0.1f);
	mat.diffuse        = glm::vec3(0.7f, 0.7f, 0.7f);
	mat.specular       = glm::vec3(1.0f, 1.0f, 1.0f);
	mat.transmittance  = glm::vec3(0.0f, 0.0f, 0.0f);
	mat.emission       = glm::vec3(0.0f, 0.0f, 0.10);
	mat.shininess      = 0.f;
	mat.ior            = 1.0f;
	mat.dissolve       = 1.f;

	mat.illum     = 1;
	mat.textureId = -1;

	mat.pbrBaseColorFactor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	mat.pbrBaseColorTexture = -1;
	mat.emissiveFactor  = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


	mat.pbrMetallicFactor   = {1.0f};
	mat.pbrMetallicRoughnessTexture = -1;

	mat.emissiveTexture = -1;

	mat.pbrRoughnessFactor  = 1.0f;

	mat.khrDiffuseFactor = { 1.0f, 1.0f, 1.0f ,1.0f } ;   // KHR_materials_pbrSpecularGlossiness
	mat.khrSpecularFactor = { 1.0f, 1.0f, 1.0f };
	mat.khrDiffuseTexture = -1;

	mat.shadingModel = MATERIAL_METALLICROUGHNESS;
	mat.khrGlossinessFactor = 0;
	mat.khrSpecularGlossinessTexture = -1;

	mat.alphaMode = {0};

	mat.alphaCutoff = { 0.5f };
	mat.doubleSided = { 0 };
	mat.normalTexture = -1;
	mat.normalTextureScale = { 1.0f };

	mat.uvTransform = glm::mat4(1);

	mat.unlit = 0;

	mat.transmissionFactor = 0;
	mat.transmissionTexture = -1;

	mat.ior = 1.33;

	mat.anisotropyDirection = {1.0f, 1.0f, 1.0f};
	mat.anisotropy = 0;

	mat.attenuationColor = {0.0f, 0.0f, 0.0f};
	mat.thicknessFactor = 0;
	mat.thicknessTexture = -1;
	mat.attenuationDistance = 0;

	mat.clearcoatFactor = 0;
	mat.clearcoatRoughness = 0;

	mat.clearcoatTexture = -1;
	mat.clearcoatRoughnessTexture = -1;
	mat.sheen = {0.0, 0.0, 0.0, 0.0};
	mat.pad;
}

GltfShadeMaterial  toMaterial(GltfMaterial& gltfm)
{
	GltfShadeMaterial m;
	initMaterial(m);
	m.type                = 1;
	m.pbrBaseColorFactor  = gltfm.baseColorFactor;
	m.emissiveFactor      = gltfm.emissiveFactor;
	m.pbrBaseColorTexture = gltfm.baseColorTexture;
	m.pbrMetallicFactor   = gltfm.metallicFactor;
	m.pbrMetallicRoughnessTexture = gltfm.metallicRoughnessTexture;

	m.emissiveTexture = gltfm.emissiveTexture;

	m.pbrRoughnessFactor  = gltfm.roughnessFactor;

	m.khrDiffuseFactor = { 1.0f, 1.0f, 1.0f ,1.0f } ;   // KHR_materials_pbrSpecularGlossiness
	m.khrSpecularFactor = { 1.0f, 1.0f, 1.0f };
	m.khrDiffuseTexture = -1;

	m.shadingModel = MATERIAL_METALLICROUGHNESS;
	m.khrGlossinessFactor = 0;
	m.khrSpecularGlossinessTexture = -1;

	m.alphaMode = gltfm.alphaMode;

	m.alphaCutoff = gltfm.alphaCutoff;
	m.doubleSided = gltfm.doubleSided;
	m.normalTexture = gltfm.normalTexture;
	m.normalTextureScale = gltfm.normalTextureScale;

	m.pbrOcclusionTexture = gltfm.occlusionTexture;
	m.pbrOcclusionTextureStrength = gltfm.occlusionTextureStrength;

	m.uvTransform;

	m.unlit = 0;

	m.transmissionFactor = gltfm.transmissionFactor;
	m.transmissionTexture = gltfm.transmissionTexture;


	m.ior = 1.33;

	vec3  anisotropyDirection;
	float anisotropy;

	vec3  attenuationColor ;
	float thicknessFactor;
	int   thicknessTexture;
	float attenuationDistance;

	float clearcoatFactor;
	float clearcoatRoughness;

	int   clearcoatTexture;
	int   clearcoatRoughnessTexture;
	vec4  sheen;
	int   pad;
	

	return m;
}

std::vector<GltfShadeMaterial> toMaterial(std::vector<MaterialObj> &objM)
{
	std::vector<GltfShadeMaterial> mVec;
	for(const auto & m : objM)
	{
		GltfShadeMaterial nowM;
		initMaterial(nowM);
		nowM.type = 0;
		nowM.ambient       = m.ambient;
		nowM.diffuse       = m.diffuse;
		nowM.specular      = m.specular;
		nowM.transmittance = m.transmittance;
		nowM.emission      = m.emission;
		nowM.shininess     = m.shininess;
		nowM.ior           = m.ior;
		nowM.dissolve      = m.dissolve;
		nowM.illum         = m.illum;
		nowM.textureId     = m.textureID;

		mVec.push_back(nowM);

	}
	return mVec;

}

void ResourceManager::loadObjModel(std::string name, std::string path, glm::mat4 transform, bool flipTexture)
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

	auto &mVec = toMaterial(loader.materials);

	model->vertexBuffer   = std::make_unique<Buffer>(device, loader.vertices, static_cast<VkBufferUsageFlagBits>( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | rayTracingFlags));
	model->indexBuffer    = std::make_unique<Buffer>(device, loader.indices, static_cast<VkBufferUsageFlagBits>( VK_BUFFER_USAGE_INDEX_BUFFER_BIT | rayTracingFlags));
	model->matColorBuffer = std::make_unique<Buffer>(device, mVec, static_cast<VkBufferUsageFlagBits>( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | flag));
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

void ResourceManager::loadScene(const std::string& filename, glm::mat4 transform, bool flipTexture)
{
	LogI("Load gltf scene: " + filename + " start");
	LogTimerStart("loadGltfScene");
	gltfLoader.loadScene(filename);

	// map gltf mesh index to my renderer mesh index
	std::map<int,int> gltfToGobalMeshMap;
	// for every mesh 
	int primMeshesIndex = 0;

	for(auto& m : gltfLoader.primMeshes)
	{
		LogProgressBar("Processing mesh" ,(double) (primMeshesIndex+1) /(double) gltfLoader.primMeshes.size() );
		std::unique_ptr<ObjModel> model = std::make_unique<ObjModel>();

		model->nbIndices  = m.indexCount;
		model->nbVertices = m.vertexCount;

		// get now mesh indices and vertexs
		std::vector<uint32_t> indices;
		std::vector<VertexObj> vertices;
		std::vector<GltfShadeMaterial> matColor;
		std::vector<int32_t> matIndex;


		// ========================== prepar mat ============================
		// get indice
		for(int i = m.firstIndex; i < m.firstIndex + m.indexCount; i++)
		{
			indices.emplace_back(gltfLoader.indices[i]);
		}

		for(int i = m.vertexOffset; i < m.vertexOffset + m.vertexCount; i++)
		{
			VertexObj v;
			v.pos      = gltfLoader.positions[i];
			v.normal   = gltfLoader.normals[i];
			v.color    = gltfLoader.colors0[i];
			v.tangent  = gltfLoader.tangents[i];
			v.bitangent = gltfLoader.bitangents[i];
			v.texCoord = gltfLoader.texcoords0[i];
			
			vertices.push_back(v);
		}


		// TODO: support real per face material
		auto& gltfmat = gltfLoader.materials[m.materialIndex];
		GltfShadeMaterial mat = toMaterial(gltfmat);

		// load texture
		auto txtOffset = static_cast<uint32_t>(textures.size());
		
		std::vector<std::string> texture;
		
		
		int32_t meshColorTxtIndex = mat.pbrBaseColorTexture;
		int32_t meshMetallicRoughnessTxtIndex = mat.pbrMetallicRoughnessTexture;
		int32_t meshEmissiveTxtIndex = mat.emissiveTexture;
		int32_t meshNormalTxtIndex = mat.normalTexture;
		int32_t meshTransmissionTxtIndex = mat.transmissionTexture;

		std::vector<tinygltf::Image*> meshTxt;

		if(meshColorTxtIndex != -1)
		{
			int sourceIndex = gltfLoader.tmodel.textures[meshColorTxtIndex].source;
			tinygltf::Image* meshColrTxt = &gltfLoader.tmodel.images[sourceIndex];

			// retarget image index
			mat.pbrBaseColorTexture = textures.size() + meshTxt.size();
			meshTxt.emplace_back(meshColrTxt);

		}


		if(meshMetallicRoughnessTxtIndex != -1 )
		{
			int sourceIndex = gltfLoader.tmodel.textures[meshMetallicRoughnessTxtIndex].source;
			tinygltf::Image* meshMetallicRoughnessTxt = &gltfLoader.tmodel.images[sourceIndex];

			// retarget image index
			mat.pbrMetallicRoughnessTexture = textures.size() + meshTxt.size();
			meshTxt.emplace_back(meshMetallicRoughnessTxt);

		}

		if(meshEmissiveTxtIndex != -1)
		{
			int sourceIndex = gltfLoader.tmodel.textures[meshEmissiveTxtIndex].source;
			tinygltf::Image* meshEmissiveTxt = &gltfLoader.tmodel.images[sourceIndex];

			// retarget image index
			mat.emissiveTexture = textures.size() + meshTxt.size();
			meshTxt.emplace_back(meshEmissiveTxt);

		}
		if(meshNormalTxtIndex != -1)
		{
			int sourceIndex = gltfLoader.tmodel.textures[meshNormalTxtIndex].source;
			tinygltf::Image* meshNormalTxt = &gltfLoader.tmodel.images[sourceIndex];

			// retarget image index
			mat.normalTexture = textures.size() + meshTxt.size();
			meshTxt.emplace_back(meshNormalTxt);

		}
		if(meshTransmissionTxtIndex != -1 )
		{
			int sourceIndex = gltfLoader.tmodel.textures[meshTransmissionTxtIndex].source;
			tinygltf::Image* meshTransmissionTxt = &gltfLoader.tmodel.images[sourceIndex];

			// retarget image index
			mat.transmissionTexture = textures.size() + meshTxt.size();
			meshTxt.emplace_back(meshTransmissionTxt);
		}

		// per face material
		// face sum no equal to vertexcount / 3 !!!
		for(int i = 0; i < m.indexCount; i++)
		{
			matColor.emplace_back(mat);
			matIndex.emplace_back(0);
		}


		createTextureImages(meshTxt,flipTexture);

		// create buffer
		// Create the buffers on Device and copy vertices , indices and materials 
		VkBufferUsageFlags flag           = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		VkBufferUsageFlags rayTracingFlags = 
		flag | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

		model->vertexBuffer   = std::make_unique<Buffer>(device, vertices, static_cast<VkBufferUsageFlagBits>( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | rayTracingFlags));
		model->indexBuffer    = std::make_unique<Buffer>(device, indices, static_cast<VkBufferUsageFlagBits>( VK_BUFFER_USAGE_INDEX_BUFFER_BIT | rayTracingFlags));
		model->matColorBuffer = std::make_unique<Buffer>(device, matColor, static_cast<VkBufferUsageFlagBits>( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | flag));
		model->matIndexBuffer = std::make_unique<Buffer>(device, matIndex, static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | flag));

		
		ObjDesc desc={};
		desc.txtOffset            = txtOffset;
		desc.vertexAddress        = model->vertexBuffer->getBufferDeviceAddress();
		desc.indexAddress         = model->indexBuffer->getBufferDeviceAddress();
		desc.materialAddress      = model->matColorBuffer->getBufferDeviceAddress();
		desc.materialIndexAddress = model->matIndexBuffer->getBufferDeviceAddress();



		gltfToGobalMeshMap[primMeshesIndex] = objModel.size();
		primMeshesIndex++;

		objModel.emplace_back(std::move(model));
		objDesc.emplace_back(std::move(desc));
		materials.emplace_back(mat);
	}

	int maxCount = 2;
	int nowCount = 0; 
	// for every node
	for(auto& node : gltfLoader.nodes)
	{
		if(node.primMesh!=-1)
		{
			ObjInstance instance;
			instance.transform = transform * node.worldMatrix ;
			instance.objIndex  = gltfToGobalMeshMap[node.primMesh];
			instance.name = gltfLoader.primMeshes[node.primMesh].name;
		
			instances.push_back(instance);
			nowCount++;
			//if(nowCount == maxCount) break;
		}
	}

	LogTimerEnd("loadGltfScene");
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

void ResourceManager::createTextureImages(const std::vector<tinygltf::Image*>& loadImages, bool flipTexture)
{
		if(loadImages.empty() && textures.empty())
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
		for(const auto& gltfImage : loadImages)
		{
			VkExtent2D                 imageSize{gltfImage->width , gltfImage->height};
			std::unique_ptr<Image>     image     = std::make_unique<Image>(device, imageSize, gltfImage->image.size(),(void*)(&gltfImage->image[0]),VK_FORMAT_R8G8B8A8_UNORM);
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

int32_t ResourceManager::getInstanceId(const std::string name)
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
	return -1;
}

}