#include"shaderInfo.h"

namespace mini
{
std::pair<uint32_t,uint32_t> ShaderInfo::findTexturePos(TEXTURE_TYPE type)
{
	for (auto& set : bindingInfoMap)
	{
		for (auto& binding : set.second) {
			if (binding.second.textureType == type) {
				return std::pair<uint32_t, uint32_t>(set.first,binding.first );
			}
		}
	}
}
}