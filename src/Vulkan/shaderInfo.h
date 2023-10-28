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
/* ����shader�ļ������Ϣ
*  ����binding��Ϣ���磺�ڼ���set�ĵڼ���binding��diffuse��ͼ��
*  δ�������Զ�����
*/
class ShaderInfo
{
public:
	std::map<uint32_t, std::map<uint32_t, BindingInfo>> bindingInfoMap;

	std::pair<uint32_t, uint32_t> findTexturePos(TEXTURE_TYPE type);
private:

};


}