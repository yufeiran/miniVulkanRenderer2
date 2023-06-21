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
class Buffer;
class Model;
class DescriptorSet;
class Queue;
class Shape;

class CommandBuffer
{
public:
	CommandBuffer(CommandPool &commandPool);

	//==============================Rendering=================================
	void begin(VkCommandBufferUsageFlags flag = {});

	void beginRenderPass(RenderPass& renderPass, FrameBuffer& frameBuffer, 
		const std::vector<VkClearValue>& clearColor);

	void bindPipeline(GraphicPipeline& pipeline);

	void setViewPortAndScissor(VkExtent2D extent);

	void bindDescriptorSet(const std::vector<std::unique_ptr<DescriptorSet>>& descriptorSets);

	void bindDescriptorSet(const std::vector<VkDescriptorSet>& descriptorSets);

	void bindVertexBuffer(Buffer& vertexBuffer);

	void bindIndexBuffer(Buffer& indexBuffer);

	void drawModel(Model& model,RenderFrame& renderFrame);

	void draw(uint32_t vertexCount,uint32_t instanceCount,uint32_t firstVertex,uint32_t firstInstance);

	void endRenderPass();

	void end();

	//==============================Rendering=================================

	//==============================Transmition===============================
	void beginSingleTime();

	void endSingleTime(Queue& queue);

	//==============================Transmition===============================

	void reset();

	void copy(Buffer& srcBuffer, Buffer& dstBuffer, VkDeviceSize size);

	void submitAndWaitIdle(Queue& queue);

	VkCommandBuffer getHandle() const;
private:
	VkCommandBuffer handle;
	Device& device;
	CommandPool& commandPool;
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	
};
}