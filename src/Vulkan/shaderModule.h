#pragma once
#include"vk_common.h"
#include"shaderInfo.h"


namespace mini
{
class Device;
class ShaderInfo;

class ShaderModule
{
public:
	ShaderModule(const std::string filename,Device& device, VkShaderStageFlagBits stage);

	~ShaderModule();

	VkShaderModule getHandle() const;

	VkShaderStageFlagBits getStageFlag() const;

	VkPipelineShaderStageCreateInfo getStageCreateInfo() const;

	void setShaderInfo(ShaderInfo& s);

	ShaderInfo& getShaderInfo();

private:
	ShaderInfo shaderInfo;

	Device& device;

	std::vector<char> loadFile(std::string filename);

	VkShaderModule handle{ VK_NULL_HANDLE };

	VkPipelineShaderStageCreateInfo stageCreateInfo{};

	std::string filename;

	std::vector<char> code;

	VkShaderStageFlagBits stage{};
};

}