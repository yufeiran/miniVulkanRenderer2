#pragma once
#include"Vulkan/vk_common.h"
#include"Common/vertex.h"
#include"Common/material.h"
#include"texture.h"

namespace mini
{

class Buffer;
class Device;
class Image;
class ImageView;

class Shape
{
public:
	Shape() = default;
	Shape(Shape&& other);

	std::string name;
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;
	Material material;

	std::unique_ptr<Buffer> vertexBuffer;
	std::unique_ptr<Buffer> indexBuffer;


	uint32_t vertexSum;
	uint32_t indexSum;


	Buffer& getVertexBuffer() const;
	Buffer& getIndexBuffer() const;

	uint32_t getVertexSum() const;
	uint32_t getIndexSum() const;
	std::string getName() const;

	std::vector<Texture>& getTextureInfos();

	std::vector<Texture> textureInfos;

	
	Texture findTextureInfo(TEXTURE_TYPE type);
private:


};
}