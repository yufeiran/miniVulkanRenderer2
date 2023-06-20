#pragma once 
#include"Vulkan/vk_common.h"
#include"tiny_obj_loader.h"

namespace mini
{

struct Material
{
	std::string name;

	float ambient[3];
	float diffuse[3];
	float transmittance[3];
	float emission[3];
	float shininess;
	float ior; // index of refraction
	float dissolve; // 1 == opaque; 0 == fully transparent

	std::string ambient_texname;                // map_Ka
	std::string diffuse_texname;                // map_Kd
	std::string specular_texname;               // map_Ks
	std::string specular_highlight_texname;     // map_Ns
};

Material toMaterial(const tinyobj::material_t& m);
}