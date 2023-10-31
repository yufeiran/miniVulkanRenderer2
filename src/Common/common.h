#pragma once 
#include<vector>
#include<map>
#include<unordered_map>
#include<cstdint>
#include<limits>
#include<algorithm>
#include<string>
#include<set>
#include<functional>
#include<assert.h>
#include<array>
#include<chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>


#define VK_USE_PLATFORM_WIN32_KHR
#include"volk.h"


#include"GLFW/glfw3.h"


#include"Common/miniLog.h"
#include"Common/miniError.h"

namespace mini
{

	const int MAX_FRAMES_IN_FLIGHT = 1;


	template<class T>
	inline std::string toString(const T& value)
	{
		std::stringstream ss;
		ss << std::fixed << value;
		return ss.str();
	}



#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			Log("Detected Vulkan error: "+toString(err),ERROR_TYPE); \
			abort();                                                \
		}                                                           \
	} while (0)

template<class T>
using BindingMap = std::map<uint32_t, std::map<uint32_t, T>>;

const VkIndexType indexType = VK_INDEX_TYPE_UINT32;


inline bool hasFlag(VkFlags item, VkFlags flag)  { return (item & flag) == flag; }

template<class integral>
constexpr integral align_up(integral x,size_t a) noexcept
{
	return integral((x + (integral(a) - 1)) & ~integral(a - 1));
}

} // namespace mini