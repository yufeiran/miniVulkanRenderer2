#pragma once 
#include"Vulkan/vk_common.h"

namespace mini
{



struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);


		return attributeDescriptions;
	}
};

class Buffer;
class Device;

class Model
{
public:
	Model(Device&device,const std::string& name,uint32_t id);
	~Model();
	


	std::string getName() const;

	uint32_t getID() const;

	Buffer& getVertexBuffer();
	Buffer& getIndexBuffer();

	uint32_t getVertexSum();
	uint32_t getIndexSum();



private:
	std::string name;
	uint32_t id;
	Device& device;
	std::unique_ptr<Buffer> vertexBuffer;
	std::unique_ptr<Buffer> indexBuffer;

	uint32_t vertexSum;
	uint32_t indexSum;

};

}