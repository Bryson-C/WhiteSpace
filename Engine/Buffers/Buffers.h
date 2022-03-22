//
// Created by Owner on 2/18/2022.
//

#ifndef ENGINE_BUFFERS_H
#define ENGINE_BUFFERS_H

#include "../../VKInclude/vulkan/vulkan.h"
#include "../Infos/Infos.hpp"
#include "../Device/Device.h"



namespace ws {

    BufferObject CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlag);

    class Buffer {
    protected:
        VkBuffer m_Buffer;
        VkDeviceMemory m_Memory;
    public:
        Buffer();

        Buffer(ws::Device device, VkDeviceSize size, VkBufferUsageFlags bufferType, void *bufferdata);

        void destroy(ws::Device device);

        VkBuffer get();
    };

    class VertexBuffer {
        VkBuffer m_Buffer, m_StagingBuffer;
        VkDeviceMemory m_Memory, m_StagingMemory;
    public:
        VertexBuffer();
        VertexBuffer(ws::Device device, VkDeviceSize size, void *vertexdata);

        void upload(ws::Device device, VkDeviceSize size, void *vertexdata);
        void create(ws::Device device, VkDeviceSize size, void *vertexdata);

        VkBuffer get();

        void destroy(ws::Device device);
    };

    class IndexBuffer : public Buffer {
    public:
        IndexBuffer();
        IndexBuffer(ws::Device device, VkDeviceSize size, void *indexdata);
        void create(ws::Device device, VkDeviceSize size, void *indexdata);
    };

    class UniformBuffer {
    private:
        vector <VkBuffer> m_Buffer;
        vector <VkDeviceMemory> m_Memory;
        VkDeviceSize m_Size;

        void create(ws::Device device, VkDeviceSize size, u32 count = 1);

    public:
        UniformBuffer();

        UniformBuffer(ws::Device device, VkDeviceSize size, u32 count = 1);

        void update(ws::Device device, void **data, u32 index);

        VkBuffer *get();

        VkDeviceSize getSize();

        vector <VkDeviceMemory> getMem();

        void destroy(ws::Device device);

        void recreate(ws::Device device, VkDeviceSize size, u32 count = 1);
    };
}

#endif //ENGINE_BUFFERS_H
