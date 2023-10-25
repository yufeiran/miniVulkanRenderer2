#include"model.h"


#include"Vulkan/buffer.h"
#include"Vulkan/device.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include"tiny_obj_loader.h"
#include"Vulkan/image.h"
#include"Vulkan/imageView.h"
#include<Windows.h>

namespace mini
{

//const std::vector<Vertex> vertices = {
//	//{{0.0f,-0.5f},{1.0f,0.0f,0.0f}},
//	//{{0.5f,0.5f},{0.0f,1.0f,0.0f}},
//	{{-0.5f,-0.5f,0.0f},{0.0f,0.0f,1.0f},{1.0f,0.0f}},
//	{ {0.5f,-0.5f,0.0f},{1.0f,1.0f,1.0f},{0.0f,0.0f}},
//	{{0.5f,0.5f,0.0f},{0.0f,1.0f,0.0f},{0.0f,1.0f}},
//	{{-0.5f,0.5f,0.0f},{0.0f,0.0f,0.0f},{1.0f,1.0f}},
//
//	{{-0.5f,-0.5f,-0.5f},{0.0f,0.0f,1.0f},{1.0f,0.0f}},
//	{ {0.5f,-0.5f,-0.5f},{1.0f,1.0f,1.0f},{0.0f,0.0f}},
//	{{0.5f,0.5f,-0.5f},{0.0f,1.0f,0.0f},{0.0f,1.0f}},
//	{{-0.5f,0.5f,-0.5f},{0.0f,0.0f,0.0f},{1.0f,1.0f}}
//
//};
//
//const std::vector<uint16_t> indices = {
//	0,1,2,2,3,0,
//	4,5,6,6,7,4
//};



Model::Model(Device& device, const std::string& filePath,const std::string& name, uint32_t id, bool flipTexture):
	device(device),name(name),filePath(filePath), id(id)
{

	Log("load Model:"+name);
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;



	auto pos=filePath.find_last_of("/");

	baseDirPath = filePath.substr(0, pos);
	baseDirPath += "/";



	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str(), baseDirPath.c_str())) {
		throw Error(warn + err);
	}

	double shapesCount = shapes.size();
	double nowShape=0;


	for (const auto& tshape : shapes) {

		std::unique_ptr<Shape> shape = std::make_unique<Shape>();


		shape->name = tshape.name;
		if(tshape.mesh.material_ids[0]>=0)
		{
			shape->material = toMaterial(materials[tshape.mesh.material_ids[0]]);
		}
		else {
			shape->material = toMaterial(materials[0]);
		}


		for (const auto& index : tshape.mesh.indices) {
			Vertex vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};
			vertex.normal={
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1-attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f,1.0f,1.0f };

			shape->vertices.push_back(vertex);
			shape->indices.push_back(shape->indices.size());

		}


		// create Vertex Buffer

		uint32_t verticesSize = sizeof(Vertex) * shape->vertices.size();
		shape->vertexSum = shape->vertices.size();

		Buffer stagingBuffer(device, verticesSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer.map(shape->vertices.data(), verticesSize);

		VkBufferUsageFlags rayTracingFlags = 
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

		shape->vertexBuffer = std::make_unique<Buffer>(device, verticesSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT| 
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT|VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR| rayTracingFlags,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		device.copyBuffer(stagingBuffer, *shape->vertexBuffer, verticesSize);

		// create Index buffer

		uint32_t indexSize = sizeof(uint32_t) * shape->indices.size();
		shape->indexSum = shape->indices.size();

		Buffer stagingBuffer1(device, indexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer1.map(shape->indices.data(), indexSize);

		shape->indexBuffer = std::make_unique<Buffer>(device, indexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
			| VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT|VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR| rayTracingFlags,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		device.copyBuffer(stagingBuffer1, *shape->indexBuffer, indexSize);

		// load texture
		auto& mat = shape->material;
		if (mat.ambient_texname != std::string()) {
			shape->textureInfos.push_back({ AMBIENT,mat.ambient_texname });
			loadImage(mat.ambient_texname, flipTexture);
		}
		if (mat.diffuse_texname != std::string()) {
			shape->textureInfos.push_back({ DIFFUSE,mat.diffuse_texname });
			loadImage(mat.diffuse_texname, flipTexture);
		}
		if (mat.emission_texname != std::string()) {
			shape->textureInfos.push_back({ SPECULAR,mat.emission_texname });
			loadImage(mat.emission_texname, flipTexture);
		}
		if (mat.specular_texname != std::string()) {
			shape->textureInfos.push_back({ EMISSION,mat.specular_texname });
			loadImage(mat.specular_texname, flipTexture);
		}
		if (mat.specular_highlight_texname != std::string()) {
			shape->textureInfos.push_back({ SPECULAR_HIGHLIGHT,mat.specular_highlight_texname });
			loadImage(mat.specular_highlight_texname, flipTexture);
		}

		shapeMap[shape->name] = std::move(shape);
		double nowProgress = nowShape/(shapesCount-1);
		LogProgressBar(" ",nowProgress);
		nowShape++;
	}
	


	Log("load model finished : total " + toString(shapeMap.size()) + " shape(s)");

	
}

Model::~Model()
{

}


std::string Model::getName() const
{
	return name;
}

uint32_t Model::getID() const
{
	return id;
}

const std::map<std::string, std::unique_ptr<Shape>>& Model::getShapeMap() const
{
	return shapeMap;
}

void Model::loadImage(const std::string& name, bool flipTexture)
{
	if (imageMap.find(name) == imageMap.end()) {
		std::string filePath = baseDirPath + name;
		imageMap[name] = std::make_unique<Image>(device, filePath, flipTexture);
		imageViewMap[name] = std::make_unique<ImageView>(*imageMap[name]);
		Log("load image:" + name);
	}
} 

ImageView& Model::getImageViewByName(const std::string& name)
{
	if (imageViewMap.find(name) != imageViewMap.end()) {
		return *imageViewMap[name];
	}
	throw Error("Can't find image by name:" + name);
}


}