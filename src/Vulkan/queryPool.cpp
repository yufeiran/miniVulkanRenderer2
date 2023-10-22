#include"queryPool.h"
#include"device.h"
using namespace mini;

QueryPool::QueryPool(Device& device,const VkQueryPoolCreateInfo& createInfo):
	device(device)
{
	vkCreateQueryPool(device.getHandle(),&createInfo,nullptr,&handle);
}

QueryPool::~QueryPool()
{
	if(handle!=VK_NULL_HANDLE)
	{
		vkDestroyQueryPool(device.getHandle(), handle, nullptr);
	}
}