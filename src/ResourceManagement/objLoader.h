#pragma once

#include "tiny_obj_loader.h"
#include "Common/common.h"
#include <array>
#include <vector>
#include <iostream>

struct MaterialObj
{
	glm::vec3  ambient        = glm::vec3(0.1f,0.1f,0.1f);
	glm::vec3  diffuse        = glm::vec3(0.7f,0.7f,0.7f);
	glm::vec3  specular       = glm::vec3(1.0f,1.0f,1.0f);
	glm::vec3  transmittance  = glm::vec3(0.0f,0.0f,0.0f);
	glm::vec3  emission       = glm::vec3(0.0f,0.0f,0.10);
	float      shininess      = 0.f;
	float      ior            = 1.0f;
	float      dissolve       = 1.f;

	int illum     = 0;
	int textureID = -1;

};

struct VertexObj
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texCoord;
};

struct shapeObj
{
	uint32_t offset;
	uint32_t nbIndex;
	uint32_t matIndex;
};

class ObjLoader
{
public:
	void loadModel(const std::string& filename);

	std::vector<VertexObj>   vertices;
	std::vector<uint32_t>    indices;
	std::vector<MaterialObj> materials;
	std::vector<std::string> textures;
	std::vector<int32_t>     matIndx;
};