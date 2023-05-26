#pragma once 
#include<vector>
#include<unordered_map>
#include<cstdint>
#include<limits>
#include<algorithm>
#include<string>
#include<set>

#define VK_USE_PLATFORM_WIN32_KHR
#include"Volk/volk.h"


#include"GLFW/glfw3.h"


#include"Common/miniLog.h"
#include"Common/miniError.h"

namespace mini
{


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

} // namespace mini