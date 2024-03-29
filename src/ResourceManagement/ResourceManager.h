#pragma once
#include "Common/common.h"
#include "Common/light.h"
#include "Vulkan/texture.h"
#include "../shaders/deviceDataStruct.h"
#include "Vulkan/sampler.h"
#include "Vulkan/commandPool.h"
#include "ResourceManagement/gltfLoader.h"


namespace mini
{
class Buffer;
class Device;
class Image;
class ImageView;
class Sampler;


// The OBJ model
struct ObjModel
{
	uint32_t nbIndices{0};
	uint32_t nbVertices{0};
	std::unique_ptr<Buffer>  vertexBuffer;
	std::unique_ptr<Buffer>  indexBuffer;
	std::unique_ptr<Buffer>  matColorBuffer;    
	std::unique_ptr<Buffer>  matIndexBuffer;
};

enum InstanceType
{
	INSTANCE_TYPE_NORMAL = 0,
	INSTANCE_TYPE_LIGHT_CUBE = 1

};

struct ObjInstance
{
	glm::mat4  transform;    // Obj matrix of the instance

	glm::vec3  translation;  // Translation of the instance
	glm::vec3  scale;        // Scale of the instance
	glm::vec3  rotation;     // Rotation of the instance

	uint32_t   objIndex{0};  // Model index reference
	std::string name;
	InstanceType type {INSTANCE_TYPE_NORMAL};
	int 	    lightIndex{-1};

	

	void updateFactorBytransform();
	void updateTransformByFactor();
};

class ResourceManager
{
public:
	ResourceManager(Device&device);
	~ResourceManager();

	void draw(CommandBuffer& cmd,PushConstantRaster& pcRaster);

	void loadLightCube();

	//return instance ID
	int  addACopyInstance(int objId, glm::mat4 transform = glm::mat4(1));

	int  addLightCubeInstance(Light& l,int lightId);

	void loadObjModel(std::string name,std::string path,glm::mat4 transform = glm::mat4(1), bool flipTexture=false);

	void loadScene(const std::string& filename, glm::mat4 transform = glm::mat4(1), bool flipTexture = false);

	void loadCubemap(const std::vector<std::string>& cubeMapNames, bool flipTexture = false);

	void createTextureImages(const std::vector<tinygltf::Image*>& loadImages, bool flipTexture = false);

	void createTextureImages(const std::vector<std::string>& textures, const std::string &modelPath, bool flipTexture = false);

	int32_t getInstanceId(const std::string name);

	int getLightCubeObjId();

	Sampler& getDefaultSampler() { return *defaultSampler; }
	Sampler& getRepeatSampler() { return *repeatSampler; }
	Sampler& getClampToEdgeSampler() { return *clampToEdgeSampler; }

	const std::vector<std::unique_ptr<ObjModel>>&  getModels() const{return objModel;}
	std::vector<ObjInstance>&     getInstances() {return instances;}

	std::vector<std::unique_ptr<ObjModel>>    objModel;
	std::vector<ObjDesc>                      objDesc;
	std::vector<ObjInstance>                  instances;
	std::vector<GltfShadeMaterial>            materials;

	std::vector<Texture>                      textures;
	std::vector<std::unique_ptr<Image>>       images;
	std::vector<std::unique_ptr<ImageView>>   imageViews;
	std::unique_ptr<Sampler>                  defaultSampler;
	std::unique_ptr<Sampler>                  repeatSampler;
	std::unique_ptr<Sampler>                  clampToEdgeSampler;

	std::unique_ptr<Image>                    cubeMap;
	std::unique_ptr<ImageView>                cubeMapView;
	Texture                                   cubeMapTexture;

	GltfLoader                                gltfLoader;

private:
	std::map<std::string, uint32_t> modelMap;

	int lightCubeObjIndex = -1;


	Device& device;
	std::unique_ptr<CommandPool> cmdPool;

};


}

