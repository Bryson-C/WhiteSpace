//
// Created by Owner on 2/18/2022.
//

#include "Buffers.h"



namespace ws {
    BufferObject CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlag) {
        BufferObject Buffer;
        auto createInfo = Info::BufferCreateInfo(size, usage);
        vkCreateBuffer(device, &createInfo, nullptr, &Buffer.Buffer);

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, Buffer.Buffer, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, memoryFlag);
        auto res = vkAllocateMemory(device, &allocInfo, nullptr, &Buffer.Memory);
        vkBindBufferMemory(device, Buffer.Buffer, Buffer.Memory, 0);
        return Buffer;
    }

    Buffer::Buffer() : m_Buffer(VK_NULL_HANDLE), m_Memory(VK_NULL_HANDLE) {}

    Buffer::Buffer(ws::Device device, VkDeviceSize size, VkBufferUsageFlags bufferType, void *bufferdata) {
        auto StartTime = clock();
        auto stagingBuffer = CREATE_STAGING_BUFFER(device.get(), device.getPhysicalDevice(), size);


        UPLOAD_BUFFER_TO_MEMORY(device.get(), stagingBuffer.Memory, bufferdata, size);


        auto Buffer = CreateBuffer(device.get(), device.getPhysicalDevice(), size,
                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferType,
                                   GPU_MEMORY);

        CopyBuffer(device, Buffer.Buffer, stagingBuffer.Buffer, size);
        m_Buffer = Buffer.Buffer;
        m_Memory = Buffer.Memory;

        vkFreeMemory(device.get(), stagingBuffer.Memory, nullptr);
        vkDestroyBuffer(device.get(), stagingBuffer.Buffer, nullptr);
        printf("Buffer: %lims\n", clock() - StartTime);
    }

    void Buffer::destroy(ws::Device device) {
        vkDestroyBuffer(device.get(), m_Buffer, nullptr);
        vkFreeMemory(device.get(), m_Memory, nullptr);
    }

    VkBuffer Buffer::get() {
        return m_Buffer;
    }

    VertexBuffer::VertexBuffer() {}

    void VertexBuffer::create(ws::Device device, VkDeviceSize size, void *vertexdata) {
        auto StartTime = clock();
        auto stagingBuffer = CREATE_STAGING_BUFFER(device.get(), device.getPhysicalDevice(), size);
        m_StagingBuffer = stagingBuffer.Buffer;
        m_StagingMemory = stagingBuffer.Memory;

        UPLOAD_BUFFER_TO_MEMORY(device.get(), stagingBuffer.Memory, vertexdata, size);


        auto Buffer = CreateBuffer(device.get(), device.getPhysicalDevice(), size,
                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                   GPU_MEMORY);

        CopyBuffer(device, Buffer.Buffer, stagingBuffer.Buffer, size);
        m_Buffer = Buffer.Buffer;
        m_Memory = Buffer.Memory;


        printf("Buffer: %lims\n", clock() - StartTime);
    }

    VertexBuffer::VertexBuffer(ws::Device device, VkDeviceSize size, void *vertexdata) {
        create(device, size, vertexdata);
    }

    void VertexBuffer::upload(ws::Device device, VkDeviceSize size, void *vertexdata) {
        auto StartTime = clock();
        destroy(device);

        auto tempBuffer = CREATE_STAGING_BUFFER(device.get(), device.getPhysicalDevice(), size);
        m_StagingBuffer = tempBuffer.Buffer;
        m_StagingMemory = tempBuffer.Memory;

        UPLOAD_BUFFER_TO_MEMORY(device.get(), m_StagingMemory, vertexdata, size);

        auto tempBufferVertex = CreateBuffer(device.get(), device.getPhysicalDevice(), size,
                                             VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, GPU_MEMORY);
        m_Buffer = tempBufferVertex.Buffer;
        m_Memory = tempBufferVertex.Memory;

        CopyBuffer(device, m_Buffer, m_StagingBuffer, size);
        printf("Vertex Buffer Recreation: %lims\n", clock() - StartTime);
    }

    VkBuffer VertexBuffer::get() { return m_Buffer; }

    void VertexBuffer::destroy(ws::Device device) {
        vkFreeMemory(device.get(), m_StagingMemory, nullptr);
        vkDestroyBuffer(device.get(), m_StagingBuffer, nullptr);
        vkDestroyBuffer(device.get(), m_Buffer, nullptr);
        vkFreeMemory(device.get(), m_Memory, nullptr);
    }

    IndexBuffer::IndexBuffer() {}

    void IndexBuffer::create(ws::Device device, VkDeviceSize size, void *indexdata) {
        auto StartTime = clock();
        auto stagingBuffer = CREATE_STAGING_BUFFER(device.get(), device.getPhysicalDevice(), size);


        UPLOAD_BUFFER_TO_MEMORY(device.get(), stagingBuffer.Memory, indexdata, size);


        auto Buffer = CreateBuffer(device.get(), device.getPhysicalDevice(), size,
                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                   GPU_MEMORY);

        CopyBuffer(device, Buffer.Buffer, stagingBuffer.Buffer, size);
        m_Buffer = Buffer.Buffer;
        m_Memory = Buffer.Memory;

        vkFreeMemory(device.get(), stagingBuffer.Memory, nullptr);
        vkDestroyBuffer(device.get(), stagingBuffer.Buffer, nullptr);
        printf("Buffer: %lims\n", clock() - StartTime);
    }

    IndexBuffer::IndexBuffer(ws::Device device, VkDeviceSize size, void *indexdata) {
        create(device, size, indexdata);
    }

    UniformBuffer::UniformBuffer() : m_Size(0), m_Memory({VK_NULL_HANDLE}), m_Buffer({VK_NULL_HANDLE}) {}

    void UniformBuffer::create(ws::Device device, VkDeviceSize size, uint32_t count) {
        auto StartTime = clock();
        m_Buffer.resize(count);
        m_Memory.resize(count);
        m_Size = size;
        for (int i = 0; i < m_Buffer.size(); i++) {
            auto buffer = CreateBuffer(device.get(), device.getPhysicalDevice(), size,
                                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, RAM_MEMORY | COHERENT_MEMORY);
            m_Buffer[i] = buffer.Buffer;
            m_Memory[i] = buffer.Memory;
        }
        printf("Uniform Buffers: %lims\n", clock() - StartTime);
    }

    UniformBuffer::UniformBuffer(ws::Device device, VkDeviceSize size, u32 count) {
        create(device, size, count);
    }

    vector <VkDeviceMemory> UniformBuffer::getMem() {
        return m_Memory;
    }

    void UniformBuffer::update(ws::Device device, void **data, u32 index) {

        if (index > m_Buffer.size())
            throw std::runtime_error("Index Was Larger Than Buffer Count");

        void *upload;
        vkMapMemory(device.get(), m_Memory[index], 0, m_Size, 0, &upload);
        memcpy(upload, data, m_Size);
        vkUnmapMemory(device.get(), m_Memory[index]);
    }

    VkBuffer *UniformBuffer::get() { return m_Buffer.data(); }

    void UniformBuffer::destroy(ws::Device device) {
        for (int i = 0; i < m_Buffer.size(); i++) {
            vkFreeMemory(device.get(), m_Memory[i], nullptr);
            vkDestroyBuffer(device.get(), m_Buffer[i], nullptr);
        }
    }

    VkDeviceSize UniformBuffer::getSize() { return m_Size; }

    void UniformBuffer::recreate(ws::Device device, VkDeviceSize size, uint32_t count) {
        create(device, size, count);
    }

}