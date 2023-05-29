#pragma once

#include"vk_common.h"
#include"framebuffer.h"

namespace mini
{
class CommandPool;
class Device;
class RenderPass;
class FrameBuffer;
class GraphicPipeline;
class RenderFrame;

class CommandBuffer
{
public:
	CommandBuffer(CommandPool &commandPool);

	void begin();

	void beginRenderPass(RenderPass& renderPass,FrameBuffer& frameBuffer,const VkClearValue &clearColor={{{0.0f,0.0f,0.0f,1.0f}}});

	void bindPipeline(GraphicPipeline& pipeline);

	void setViewPortAndScissor(VkExtent2D extent);

	void draw(uint32_t vertexCount,uint32_t instanceCount,uint32_t firstVertex,uint32_t firstInstance);

	void endRenderPass();

	void end();

	void reset();

	VkCommandBuffer getHandle() const;
private:
	VkCommandBuffer handle;
	Device& device;
	CommandPool& commandPool;
	
};
}