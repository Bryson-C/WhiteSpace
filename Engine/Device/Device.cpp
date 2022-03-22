//
// Created by Owner on 2/18/2022.
//

#include "Device.h"


namespace ws {

    VkPhysicalDevice                    FindPhysicalDevice(VkInstance instance) {
                u32 count = 0;
                vkEnumeratePhysicalDevices(instance, &count, nullptr);
                vector<VkPhysicalDevice> devices(count);
                vkEnumeratePhysicalDevices(instance, &count, devices.data());
                for (auto i: devices) {
                    return i;
                }
            }
    vector<VkQueueFamilyProperties>     FindQueueFamilyProperties(VkPhysicalDevice physicalDevice) {
                u32 count = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
                vector<VkQueueFamilyProperties> queueFamilies(count);
                vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilies.data());
                return queueFamilies;
            }
    bool                                deviceSupportsSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, u32 index) {
                VkBool32 support;
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, surface, &support);
                return support;
            }

                            Device::Device(ws::Instance& instance, Window& window) {
        auto StartTime = clock();
        m_PhysicalDevice = FindPhysicalDevice(instance.get());
        auto queueFamilyProps = FindQueueFamilyProperties(m_PhysicalDevice);

        u32 loop = 0;
        for (auto& i : queueFamilyProps) {
            if (i.queueFlags & qGRAPHICS_FLAG) { m_GraphicsIndex = loop; }
            if (deviceSupportsSurface(m_PhysicalDevice, window.getSurface(), loop)) { m_PresentIndex = loop; }
            if (i.queueFlags & qTRANSFER_FLAG && i.queueFlags & ~qGRAPHICS_FLAG) m_DedicatedTransfer = loop;
            loop++;
        }
        const float priority = 1.0f;
        vector<VkDeviceQueueCreateInfo> qInfos{};
        qInfos.emplace_back(Info::QueueCreateInfo(m_GraphicsIndex, 1, &priority));
        qInfos.emplace_back(Info::QueueCreateInfo(m_PresentIndex, 1, &priority));
        vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkPhysicalDeviceFeatures features{};
        features.samplerAnisotropy = VK_TRUE;
        auto deviceInfo = Info::DeviceCreateInfo(qInfos, {}, extensions, &features);
        vkCreateDevice(m_PhysicalDevice, &deviceInfo, nullptr, &m_Device);

        vkGetDeviceQueue(m_Device, m_GraphicsIndex, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, m_PresentIndex, 0, &m_PresentQueue);
        vkGetDeviceQueue(m_Device, m_DedicatedTransfer, 0, &m_TransferQueue);

        m_TransferBuffers.resize(5);
        VkCommandPoolCreateInfo poolInfo = Info::CommandPoolCreateInfo(m_DedicatedTransfer, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
        auto poolResult = vkCreateCommandPool(m_Device, &poolInfo, VK_NULL_HANDLE, &m_TransferPool);
        VkCommandBufferAllocateInfo allocInfo = Info::CommandBufferAllocationInfo(m_TransferPool, m_TransferBuffers.size(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        auto allocResult = vkAllocateCommandBuffers(m_Device, &allocInfo, m_TransferBuffers.data());
        m_TransferFences = createFences(m_TransferBuffers.size(), 0);
        vkResetFences(m_Device, m_TransferFences.size(), m_TransferFences.data());

        if (poolResult != SUCCESS) throw std::runtime_error("Failed To Create Transfer Pool");
        if (allocResult != SUCCESS) throw std::runtime_error("Failed To Allocate Transfer Buffers");

        printf("Device: %lims\n", clock()-StartTime);
    }
    VkDevice                Device::get() { return m_Device; }
    VkPhysicalDevice        Device::getPhysicalDevice() { return m_PhysicalDevice; }
    u32                     Device::getGraphicsIndex() { return m_GraphicsIndex; }
    VkQueue                 Device::getGraphicsQueue() { return m_GraphicsQueue; }
    VkQueue                 Device::getTransferQueue() { return m_TransferQueue; }
    VkCommandPool           Device::getPool() { return m_TransferPool; }
    vector<VkCommandBuffer> Device::getTransferBuffers() { return m_TransferBuffers; }
    vector<VkFence>         Device::getTransferFences() { return m_TransferFences; }
    VkResult                Device::submitQueue(VkSubmitInfo& submitInfo, VkFence fence) {
        return vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, fence);
    }
    VkResult                Device::presentQueue(VkPresentInfoKHR& presentInfo) {
        return vkQueuePresentKHR(m_GraphicsQueue, &presentInfo);
    }
    VkResult                Device::waitForFences(VkFence* fences, u32 count) {
        return vkWaitForFences(m_Device, count, fences, VK_TRUE, UINT64_MAX);
    }
    VkResult                Device::resetFences(VkFence* fences, u32 count) {
        return vkResetFences(m_Device, count, fences);
    }
    vector<VkSemaphore>     Device::createSemaphores(uint32_t count) {
        vector<VkSemaphore> semaphores(count, nullptr);
        for (auto& i : semaphores) {
            auto info = Info::SemaphoreCreateInfo();
            auto res = vkCreateSemaphore(m_Device, &info, nullptr, &i);
        }
        return semaphores;
    }
    vector<VkFence>         Device::createFences(uint32_t count, VkFenceCreateFlags flags) {
        vector<VkFence> fences(count);
        for (auto& i : fences) {
            auto info = Info::FenceCreateInfo(flags);
            auto res = vkCreateFence(m_Device, &info, nullptr, &i);
        }
        return fences;
    }
    void                    Device::waitIdle() {
        vkDeviceWaitIdle(m_Device);
    }

    void                    CopyBuffer(ws::Device device, VkBuffer dst, VkBuffer src, VkDeviceSize size) {
        auto allocInfo = Info::CommandBufferAllocationInfo(device.getPool(), 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        bool selected = false;
        VkCommandBuffer selectedBuffer;
        VkFence selectedFence;
        for (int i = 0; i < device.getTransferBuffers().size(); i++) {
            //if (vkGetFenceStatus(device.get(), device.getTransferFences()[i]) == VK_NOT_READY) continue;
            //else printf("Success! Fence Is Ready!\n");
            selectedBuffer = device.getTransferBuffers()[i];
            selectedFence = device.getTransferFences()[i];
            selected = true;
        }

        VkCommandBufferBeginInfo beginfo = Info::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        vkBeginCommandBuffer(selectedBuffer, &beginfo);
        VkBufferCopy copyRegion = Info::BufferCopyInfo(0, 0, size);
        vkCmdCopyBuffer(selectedBuffer, src, dst, 1, &copyRegion);
        vkEndCommandBuffer(selectedBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &selectedBuffer;

        vkQueueSubmit(device.getTransferQueue(), 1, &submitInfo, selectedFence);
        vkQueueWaitIdle(device.getTransferQueue());
        vkWaitForFences(device.get(), 1, &selectedFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device.get(), 1, &selectedFence);
        vkResetCommandBuffer(selectedBuffer, 0);
    }

}