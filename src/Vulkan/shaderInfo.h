#pragma once

#include "Common/common.h"

namespace mini
{
enum BindingType {
	BUFFER_BINDING_TYPE = 0,
	TEXTURE_BINDING_TYPE

};
enum TEXTURE_TYPE{
	DIFFUSE =0
};

class BindingInfo {
public:
	BindingInfo(BindingType type, TEXTURE_TYPE textureType) :type(type), textureType(textureType) {};
	BindingInfo() = default;
	BindingType type;
	TEXTURE_TYPE textureType;

};

// 
/* 保存shader文件里的信息
*  比如binding信息（如：第几个set的第几个binding是diffuse贴图）
*  未来考虑自动解析
*/
class ShaderInfo
{
public:
	std::map<uint32_t, std::map<uint32_t, BindingInfo>> bindingInfoMap;

	std::pair<uint32_t, uint32_t> findTexturePos(TEXTURE_TYPE type);
private:

};


}