#include "shaderModule.h"
#include"device.h"
#include<fstream>

namespace mini
{
ShaderModule::ShaderModule(const std::string filename, Device& device, VkShaderStageFlagBits stage)
	:filename(filename), stage(stage),device(device)
{
	code = loadFile(filename);
	VkShaderModuleCreateInfo createInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };

	createInfo.pCode = reinterpret_cast<const uint32_t*> (code.data());
	createInfo.codeSize = code.size();
	if (vkCreateShaderModule(device.getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
		throw Error("Failed to create shader Module!");
	}

	stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageCreateInfo.stage = stage;
	stageCreateInfo.module = handle;
	stageCreateInfo.pName = "main";

}

ShaderModule::~ShaderModule()
{
	if (handle != VK_NULL_HANDLE) {
		vkDestroyShaderModule(device.getHandle(), handle, nullptr);
	}
}

VkShaderModule ShaderModule::getHandle() const
{
	return handle;
}

VkShaderStageFlagBits ShaderModule::getStageFlag() const
{
	return stage;
}

VkPipelineShaderStageCreateInfo ShaderModule::getStageCreateInfo() const
{
	return stageCreateInfo;
}

void ShaderModule::setShaderInfo(ShaderInfo& s)
{
	shaderInfo = s;
}

ShaderInfo& ShaderModule::getShaderInfo()
{
	return shaderInfo;
}



std::vector<char> ShaderModule::loadFile(std::string filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw Error(std::string("Failed to open file:" + filename).c_str());
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char>buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

}
