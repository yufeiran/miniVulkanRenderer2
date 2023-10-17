#include"ResourceManagement/postQuad.h"

using namespace mini;

PostQuad::PostQuad(Device& device)
{
	const glm::vec3 quadVerticesPos[] = {
        // positions        // texture Coords
        { -1.0f,  1.0f, 0.0f},
        {-1.0f, -1.0f, 0.0f}, 
        {1.0f,  1.0f, 0.0f}, 
        {1.0f, -1.0f, 0.0f}
    };
    const glm::vec2 quadVerticesCoords[]={
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 1.0f},
        {1.0f, 0.0f}
    };

    for(int i=0;i<4;i++)
    {
	    Vertex v;
        v.pos=quadVerticesPos[i];
        v.color={1.0f,1.0f,1.0f};
        v.texCoord=quadVerticesCoords[i];
        vertexs.push_back(v);
    }
    uint32_t verticesSize = sizeof(Vertex)*vertexs.size();
    uint32_t verticesSum=vertexs.size();

    Buffer stagingBuffer(device,verticesSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.map(vertexs.data(),verticesSize);

    vertexBuffer = std::make_unique<Buffer>(device,verticesSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device.copyBuffer(stagingBuffer,*vertexBuffer,verticesSize);
}
