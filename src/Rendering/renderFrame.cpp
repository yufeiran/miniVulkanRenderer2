#include "renderFrame.h"
#include"Vulkan/device.h"
#include"renderTarget.h"


namespace mini
{
RenderFrame::RenderFrame(Device& device, std::unique_ptr<RenderTarget>&& renderTarget)
	:device(device),swapchainRenderTarget(std::move(renderTarget))
{

}



Device& RenderFrame::getDevice()
{
	return device;
}

}
