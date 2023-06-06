// 用来存同步在GPU内存和CPU内存的结构,比如uniform结构
#pragma once
#include"Vulkan/vk_common.h"


struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};