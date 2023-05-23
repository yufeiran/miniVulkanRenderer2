#pragma once
#include"vk_common.h"



namespace mini
{


class Device;
class CommandBuffer;

class Queue
{
public:
	Queue(Device &device,uint32_t familyIndex,VkQueueFamilyProperties properties,VkBool32 canPresent,uint32_t index);

	Queue(Queue&& other);

	const Device& getDevice() const;

	VkQueue getHandle() const;

	uint32_t getFamilyIndex() const;

	uint32_t getIndex() const;

	const VkQueueFamilyProperties& getProperties() const;

	VkBool32 supportPresent();

	VkResult submit(const std::vector<VkSubmitInfo>& submitInfos, VkFence fence) const;

	VkResult submit(const CommandBuffer& commandBuffer, VkFence fence) const;

	VkResult present(const VkPresentInfoKHR& presentInfos) const;

	VkResult waitIdle() const;

private:
	Device& device;

	VkQueue handle{ VK_NULL_HANDLE };

	uint32_t familyIndex{ 0 };

	uint32_t index{ 0 };

	VkBool32 canPresent{ VK_FALSE };

	VkQueueFamilyProperties properties{};
};
}