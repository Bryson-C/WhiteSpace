//
// Created by Owner on 2/18/2022.
//

#include "CommandPool.h"

namespace ws {
                CommandBuffer::CommandBuffer() {}
                CommandBuffer::CommandBuffer(ws::Device &device, VkCommandPool* owner, uint32_t count) : m_OwnerPool(owner), m_Index(0) {
    auto StartTime = clock();
    m_Buffers.resize(count);
    auto info = Info::CommandBufferAllocationInfo(*m_OwnerPool, count, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    auto res = vkAllocateCommandBuffers(device.get(), &info, m_Buffers.data());
    printf("Command Buffers: %lims\n", clock()-StartTime);
}
void            CommandBuffer::nextIndex() { if (m_Index < m_Buffers.size()-1) m_Index++; }
void            CommandBuffer::prevIndex() { if (m_Index != 0) m_Index--; }
void            CommandBuffer::setIndex(u32 index) { m_Index = index; }
void            CommandBuffer::record(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent) {
    RecordCommandBuffer(m_Buffers[m_Index]);
    BeginRenderPass(extent, renderPass, framebuffer, m_Buffers[m_Index]);
}
void            CommandBuffer::stopRecording() {
    EndRenderPass(m_Buffers[m_Index]);
    EndCommandBuffer(m_Buffers[m_Index]);
}
void            CommandBuffer::bindPipeline(VkPipeline pipeline, VkPipelineBindPoint bindPoint) {
    vkCmdBindPipeline(m_Buffers[m_Index], bindPoint, pipeline);
}
void            CommandBuffer::draw(u32 vertexCount, u32 instances, u32 firstVertex, u32 firstInstance) {
    vkCmdDraw(m_Buffers[m_Index], vertexCount, instances, firstVertex, firstInstance);
}
void            CommandBuffer::drawIndexed(u32 indexCount, u32 instances, u32 firstIndex , u32 firstVertex, u32 firstInstance) {
    vkCmdDrawIndexed(m_Buffers[m_Index], indexCount, instances, firstIndex, firstVertex, firstInstance);
}

// TODO: Implement
void            CommandBuffer::drawIndexedIndirect(uint32_t indexCount, uint32_t instances, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) {

        //ws::Buffer buffer(device, pool, );

        VkDrawIndexedIndirectCommand cmd{};
        cmd.instanceCount = instances;
        cmd.firstIndex = firstIndex;
        cmd.firstInstance = firstInstance;
        cmd.indexCount = indexCount;
        cmd.vertexOffset = vertexOffset;
        //vkCmdDrawIndexedIndirect(m_Buffers[m_Index], , );

}
void            CommandBuffer::bindVertexBuffer(VkBuffer buffer) {
    VkDeviceSize offset = {0};
    vkCmdBindVertexBuffers(m_Buffers[m_Index], 0, 1, &buffer, &offset);
}
void            CommandBuffer::bindIndexBuffer(VkBuffer buffer, VkIndexType type) {
    VkDeviceSize offset = {0};
    vkCmdBindIndexBuffer(m_Buffers[m_Index], buffer, offset, type);
}
void            CommandBuffer::bindDesciptorSets(VkPipelineLayout layout, VkDescriptorSet set, VkPipelineBindPoint stage) {
    vkCmdBindDescriptorSets(m_Buffers[m_Index], stage, layout, 0, 1, &set, 0, nullptr);
}
void            CommandBuffer::bindPushConstants(VkPipelineLayout layout, PipelineAccess access, void* data, u32 size, u32 offset) {
    vkCmdPushConstants(m_Buffers[m_Index], layout, access, offset, size, data);
}

VkCommandBuffer CommandBuffer::get(uint32_t index) {
    return m_Buffers[index];
}
void            CommandBuffer::freeBuffers(ws::Device device) {
    vkFreeCommandBuffers(device.get(), *m_OwnerPool, m_Buffers.size(), m_Buffers.data());
}




                CommandPool::CommandPool(ws::Device device) {
    auto StartTime = clock();
    auto createInfo = Info::CommandPoolCreateInfo(device.getGraphicsIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    vkCreateCommandPool(device.get(), &createInfo, nullptr, &m_Pool);
    printf("Command Pool: %lims\n", clock()-StartTime);
}
CommandBuffer   CommandPool::AllocateBuffers(ws::Device& device, u32 count) {
    auto buffers = CommandBuffer(device, &m_Pool, count);
    m_Buffers = &buffers;
    return buffers;
}
VkCommandPool   CommandPool::get() {
    return m_Pool;
}
}