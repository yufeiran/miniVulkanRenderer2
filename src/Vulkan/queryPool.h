#include"Common/common.h"

namespace mini
{
class Device;

class QueryPool
{
public:
	QueryPool(Device& device,const VkQueryPoolCreateInfo& createInfo);
	~QueryPool();
	VkQueryPool getHandle() const{return handle;}
private:
	VkQueryPool handle = VK_NULL_HANDLE;
	Device& device;

};

}