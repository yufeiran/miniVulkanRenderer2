#include"shape.h"
#include"Vulkan/buffer.h"
#include"Vulkan/imageView.h"
#include"Vulkan/image.h"

namespace mini
{
	Shape::Shape(Shape&& other):name(other.name),vertices(other.vertices),indices(other.indices),material(other.material),
		vertexBuffer(std::move( other.vertexBuffer)),indexBuffer(std::move(other.indexBuffer)),vertexSum(other.vertexSum),
		indexSum(other.indexSum)
	{

	}

	Buffer& Shape::getVertexBuffer() const
	{
		return *vertexBuffer;
	}

	Buffer& Shape::getIndexBuffer() const 
	{
		return *indexBuffer;
	}

	uint32_t Shape::getVertexSum() const 
	{
		return vertexSum;
	}

	uint32_t Shape::getIndexSum() const
	{
		return indexSum;
	}

	std::string Shape::getName() const
	{
		return name;
	}

	std::vector<Texture>& Shape::getTextureInfos()
	{
		return textureInfos;
	}

Texture Shape::findTextureInfo(TEXTURE_TYPE type)
{
	for (auto& t : textureInfos)
	{
		if (t.type == type)
		{
			return t;
		}
	}
	return Texture(NONE,"");
}


}