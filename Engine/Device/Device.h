//
// Created by Owner on 2/18/2022.
//

#ifndef ENGINE_DEVICE_H
#define ENGINE_DEVICE_H

#include "../../VKInclude/vulkan/vulkan.h"
#include "../../GLFW/include/glfw3.h"
#include "../Instance/Instance.h"
#include "../Window/Window.h"
#include "../Infos/Infos.hpp"

namespace ws {



    class Device {
    private:
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        u32 m_GraphicsIndex, m_PresentIndex, m_DedicatedTransfer;
        VkQueue m_GraphicsQueue, m_PresentQueue, m_TransferQueue;

        VkCommandPool m_TransferPool;
        std::vector<VkCommandBuffer> m_TransferBuffers;
        std::vector<VkFence> m_TransferFences;
    public:
        Device(ws::Instance& instance, Window& window);
        VkDevice get();
        VkPhysicalDevice getPhysicalDevice();
        u32 getGraphicsIndex();
        VkQueue getGraphicsQueue();
        VkQueue getTransferQueue();
        VkCommandPool getPool();
        vector<VkCommandBuffer> getTransferBuffers();
        vector<VkFence> getTransferFences();
        VkResult submitQueue(VkSubmitInfo& submitInfo, VkFence fence);
        VkResult presentQueue(VkPresentInfoKHR& presentInfo);
        VkResult waitForFences(VkFence* fences, u32 count);
        VkResult resetFences(VkFence* fences, u32 count);
        vector<VkSemaphore> createSemaphores(u32 count);
        vector<VkFence> createFences(u32 count, VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);
        void waitIdle();
    };

    void CopyBuffer(ws::Device device, VkBuffer dst, VkBuffer src, VkDeviceSize size);

}

#endif //ENGINE_DEVICE_H
