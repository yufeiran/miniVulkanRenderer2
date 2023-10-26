#pragma once
#include"Common/common.h"
#include"Common/vertex.h"
#include"Vulkan/buffer.h"
#include"Vulkan/device.h"

namespace mini
{


class PostQuad
{
public:
	PostQuad(Device&device);
	Buffer& getVertexBuffer() const{return *vertexBuffer;}
	int getVertexCount() const {return vertexs.size();}
private:
	std::vector<Vertex> vertexs;
	std::unique_ptr<Buffer> vertexBuffer;
};

}
