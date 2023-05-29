#include"fence.h"
#include"device.h"

namespace mini
{
Fence::Fence(Device& device, VkFenceCreateFlags flag)
	:device(device)
{
	VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	fenceInfo.flags = flag;

	if (vkCreateFence(device.getHandle(), &fenceInfo, nullptr, &handle) != VK_SUCCESS) {
		throw Error("Failed to create fence");
	}
}

Fence::~Fence()
{
	if (handle != VK_NULL_HANDLE) {
		vkDestroyFence(device.getHandle(), handle, nullptr);
	}
}

void Fence::wait()
{
	vkWaitForFences(device.getHandle(), 1, &handle, VK_TRUE, UINT64_MAX);
}

void Fence::reset()
{
	vkResetFences(device.getHandle(), 1, &handle);
}

VkFence Fence::getHandle() const
{
	return handle;
}
}