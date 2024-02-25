#pragma once

#include"Common/common.h"
#include"framebuffer.h"
#include"../shaders/deviceDataStruct.h"

namespace mini
{
class CommandPool;
class Device;
class RenderPass;
class FrameBuffer;
class GraphicsPipeline;
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

	void bindPipeline(const GraphicsPipeline& pipeline);

	void setViewPortAndScissor(VkExtent2D extent);

	void bindDescriptorSet(const std::vector<std::unique_ptr<DescriptorSet>>& descriptorSets);

	void bindDescriptorSet(const std::vector<VkDescriptorSet>& descriptorSets);

	void bindDescriptorSet(const VkDescriptorSet& descriptorSet);

	void bindVertexBuffer(Buffer& vertexBuffer);

	void bindIndexBuffer(Buffer& indexBuffer);

	void pushConstant(PushConstantRaster& pushConstantsMesh,VkShaderStageFlagBits stage);

	void nextSubpass();


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