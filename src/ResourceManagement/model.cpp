#include"model.h"
#include"Vulkan/buffer.h"
#include"Vulkan/device.h"

namespace mini
{

const std::vector<Vertex> vertices = {
	//{{0.0f,-0.5f},{1.0f,0.0f,0.0f}},
	//{{0.5f,0.5f},{0.0f,1.0f,0.0f}},
	{{-0.5f,-0.5f},{0.0f,0.0f,1.0f},{1.0f,0.0f}},
	{ {0.5f,-0.5f},{1.0f,1.0f,1.0f},{0.0f,0.0f}},
	{{0.5f,0.5f},{0.0f,1.0f,0.0f},{0.0f,1.0f}},
	{{-0.5f,0.5f},{0.0f,0.0f,0.0f},{1.0f,1.0f}}

};

const std::vector<uint16_t> indices = {
	0,1,2,2,3,0
};

Model::Model(Device& device,const std::string& name, uint32_t id):
	device(device),name(name),id(id), vertexSum(0)
{



	// create Vertex Buffer

	uint32_t verticesSize = sizeof(Vertex) * vertices.size();
	vertexSum = vertices.size();

	Buffer stagingBuffer(device, verticesSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.map(vertices.data(), verticesSize);

	vertexBuffer = std::make_unique<Buffer>(device, verticesSize,VK_BUFFER_USAGE_TRANSFER_DST_BIT| VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	device.copyBuffer(stagingBuffer, *vertexBuffer, verticesSize);

	// create Index buffer

	uint32_t indexSize = sizeof(uint16_t) * indices.size();
	indexSum = indices.size();

	Buffer stagingBuffer1(device, indexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer1.map(indices.data(), indexSize);

	indexBuffer = std::make_unique<Buffer>(device, indexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	device.copyBuffer(stagingBuffer1, *indexBuffer, indexSize);


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

Buffer& Model::getVertexBuffer()
{
	return *vertexBuffer;
}

Buffer& Model::getIndexBuffer()
{
	return *indexBuffer;
}

uint32_t Model::getVertexSum()
{
	return vertexSum;
}

uint32_t Model::getIndexSum()
{
	return indexSum;
}

}