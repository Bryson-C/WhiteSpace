//
// Created by Owner on 2/18/2022.
//

#ifndef ENGINE_COMMANDPOOL_H
#define ENGINE_COMMANDPOOL_H

#include "../../VKInclude/vulkan/vulkan.h"
#include "../Infos/Infos.hpp"
#include "../Device/Device.h"
#include "../Buffers/Buffers.h"


namespace ws {


    class CommandBuffer {
    private:
        VkCommandPool* m_OwnerPool;
        vector<VkCommandBuffer> m_Buffers;
        u32 m_Index;

    public:
        CommandBuffer();
        CommandBuffer(ws::Device& device, VkCommandPool* owner, u32 count);
        void nextIndex();
        void prevIndex();
        void setIndex(u32 index);

        void record(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent);
        void stopRecording();
        void bindPipeline(VkPipeline pipeline, VkPipelineBindPoint bindPoint);
        void draw(u32 vertexCount, u32 instances, u32 firstVertex, u32 firstInstance);
        void drawIndexed(u32 indexCount, u32 instances, u32 firstIndex, u32 firstVertex, u32 firstInstance);
        void drawIndexedIndirect(u32 indexCount, u32 instances, u32 firstIndex, u32 vertexOffset, u32 firstInstance);
        void bindVertexBuffer(VkBuffer buffer);
        void bindIndexBuffer(VkBuffer buffer, VkIndexType type = VK_INDEX_TYPE_UINT16);
        void bindDesciptorSets(VkPipelineLayout layout, VkDescriptorSet set, VkPipelineBindPoint stage = VK_PIPELINE_BIND_POINT_GRAPHICS);
        void bindPushConstants(VkPipelineLayout layout, PipelineAccess access, void* data, u32 size, u32 offset = 0);

        VkCommandBuffer get(u32 index);

        void freeBuffers(ws::Device device);
    };


    class CommandPool {
    private:
        VkCommandPool m_Pool;
        CommandBuffer* m_Buffers;
    public:
        CommandPool(ws::Device device);
        CommandBuffer AllocateBuffers(ws::Device& device, u32 count);
        VkCommandPool get();
        void destroy(ws::Device device);
    };




}

#endif //ENGINE_COMMANDPOOL_H
