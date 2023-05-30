#include"framebuffer.h"
#include"commandBuffer.h"
#include"commandPool.h"
#include"device.h"
#include"Rendering/renderPass.h"
#include"graphicPipeline.h"
#include"buffer.h"
#include"ResourceManagement/model.h"

namespace mini
{
CommandBuffer::CommandBuffer(CommandPool& commandPool)
	:device(commandPool.getDevice()),commandPool(commandPool)
{

    VkCommandBufferAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocInfo.commandPool = commandPool.getHandle();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device.getHandle(), &allocInfo, &handle) != VK_SUCCESS) {
        throw Error("Failed to allocate command buffers!");
    }
}

void CommandBuffer::begin(VkCommandBufferUsageFlags flag)
{
    VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = flag;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(handle, &beginInfo) != VK_SUCCESS) {
        throw Error("Failed to begin recording command buffer!");
    }
}

void CommandBuffer::beginRenderPass(RenderPass& renderPass, FrameBuffer& frameBuffer,const VkClearValue& clearColor)
{
    VkRenderPassBeginInfo renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    renderPassBeginInfo.renderPass = renderPass.getHandle();
    renderPassBeginInfo.framebuffer = frameBuffer.getHandle();
    renderPassBeginInfo.renderArea.offset = { 0,0 };
    renderPassBeginInfo.renderArea.extent = frameBuffer.getExtent();

    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(handle, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

}

void CommandBuffer::bindPipeline(GraphicPipeline& pipeline)
{
    vkCmdBindPipeline(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getHandle());
}

void CommandBuffer::setViewPortAndScissor(VkExtent2D extent)
{
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(handle, 0, 1, &viewport);
    
    VkRect2D scissor{};
    scissor.offset = { 0,0 };
    scissor.extent = extent;
    vkCmdSetScissor(handle, 0, 1, &scissor);
}

void CommandBuffer::bindVertexBuffer(Buffer& vertexBuffer)
{
    VkBuffer vertexBuffers[] = { vertexBuffer.getHandle() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(handle, 0, 1, vertexBuffers, offsets);
}

void CommandBuffer::bindIndexBuffer(Buffer& indexBuffer)
{
    // TODO: 类型有可能是UINT32 或 UINT16
    vkCmdBindIndexBuffer(handle, indexBuffer.getHandle(), 0, VK_INDEX_TYPE_UINT16);
}

void CommandBuffer::drawModel(Model& model)
{
    auto& vertexBuffer = model.getVertexBuffer();
    auto& indexBuffer = model.getIndexBuffer();

    bindVertexBuffer(vertexBuffer);
    bindIndexBuffer(indexBuffer);

    vkCmdDrawIndexed(handle, model.getIndexSum(), 1, 0, 0, 0);

}

void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    vkCmdDraw(handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::endRenderPass()
{
    vkCmdEndRenderPass(handle);
}

void CommandBuffer::end()
{
    if (vkEndCommandBuffer(handle) != VK_SUCCESS) {
        throw Error("Failed to record command buffer!");
    }
}

void CommandBuffer::reset()
{
    vkResetCommandBuffer(handle, 0);
}

void CommandBuffer::copy(Buffer& srcBuffer, Buffer& dstBuffer, VkDeviceSize size)
{
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(handle, srcBuffer.getHandle(), dstBuffer.getHandle(), 1, &copyRegion);
}

VkCommandBuffer CommandBuffer::getHandle() const
{
    return handle;
}

}

