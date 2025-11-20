#pragma once 
#include <vector>
#include <map>
#include <unordered_map>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <string>
#include <set>
#include <functional>
#include <assert.h>
#include <array>
#include <chrono>
#include <memory>
#include <sstream>
#include <cstring>
#include "miniLog.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <glm/gtc/matrix_transform.hpp>


#if defined(_WIN32) || defined(_WIN64)
#define VK_USE_PLATFORM_WIN32_KHR
#elseif defined(UNIX)
#define VK_USE_PLATFORM_XCB_KHR
#endif
#include"volk.h"


#include"GLFW/glfw3.h"


#include"Common/miniLog.h"
#include"Common/miniError.h"
#include"Common/assertPath.h"

namespace mini
{

	const int MAX_FRAMES_IN_FLIGHT = 2;


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


template <typename T, typename TFlag>
inline bool hasFlag(T item, TFlag flag)  { return (item & flag) == flag; }

template<class integral>
constexpr integral align_up(integral x,size_t a) noexcept
{
	return integral((x + (integral(a) - 1)) & ~integral(a - 1));
}

inline bool isDepthOnlyFormat(VkFormat format) {
	switch (format) {
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_D32_SFLOAT:
			return true;
		default:
			return false;
		}
}

inline bool isDepthStencilFormat(VkFormat format) {
	switch (format) {
			case VK_FORMAT_D16_UNORM_S8_UINT:
			case VK_FORMAT_D24_UNORM_S8_UINT:
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
				return true;
			default:
				return isDepthOnlyFormat(format);
			}
}


inline float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}
} // namespace mini