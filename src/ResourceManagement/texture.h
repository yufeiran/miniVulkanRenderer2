#pragma once 
#include"Vulkan/vk_common.h"


namespace mini
{

enum TEXTURE_TYPE
{
	AMBIENT = 0,
	DIFFUSE,
	SPECULAR,
	SPECULAR_HIGHLIGHT
};

class Texture
{
public:
	Texture(TEXTURE_TYPE type, const std::string& name) :type(type), name(name) {};
	TEXTURE_TYPE type;
	std::string name; // or path

};

}