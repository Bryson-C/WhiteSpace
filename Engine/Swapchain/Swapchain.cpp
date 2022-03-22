//
// Created by Owner on 2/18/2022.
//

#include "Swapchain.h"

namespace ws {
    VkSurfaceFormatKHR getSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
        u32 count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
        vector<VkSurfaceFormatKHR> formats(count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, formats.data());
        for (auto &i: formats) {
            if (i.format == VK_FORMAT_B8G8R8A8_SRGB && i.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return i;
            }
        }
        return formats[0];
    }

    VkPresentModeKHR getPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
        u32 count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
        vector<VkPresentModeKHR> presentModes(count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, presentModes.data());
        for (auto &i: presentModes) {
            if (i == VK_PRESENT_MODE_MAILBOX_KHR) return i;
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    u32 getMinimumImageCount(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
        u32 imageCount = 0;
        VkSurfaceCapabilitiesKHR capa;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capa);
        imageCount = capa.minImageCount + 1;
        if (capa.maxImageCount > 0 && imageCount > capa.maxImageCount) {
            imageCount = capa.maxImageCount;
        }
        return imageCount;
    }

    ImageArray CreateSwapchainImagesAndViews(VkDevice device, VkSwapchainKHR swapchain, VkFormat format) {
        ImageArray Images;
        u32 imageCount = 0;
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
        Images.Image.resize(imageCount);
        Images.View.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, Images.Image.data());

        for (u32 i = 0; i < Images.Image.size(); i++) {
            auto range = Info::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
            auto component = Info::ComponentMapping();
            auto info = Info::ImageViewCreateInfo(Images.Image[i], VK_IMAGE_VIEW_TYPE_2D, format, component, range);
            vkCreateImageView(device, &info, nullptr, &Images.View[i]);
        }
        return Images;
    }


    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        for (const auto &availableFormat: availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        for (const auto &availablePresentMode: availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                            capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void ws::Swapchain::create(ws::Device device, ws::Window window) {
        auto StartTime = clock();
        m_Formats.Extent = {window.size().width, window.size().height};
        m_Formats.Format = getSurfaceFormat(device.getPhysicalDevice(), window.getSurface());
        m_Formats.PresentMode = getPresentMode(device.getPhysicalDevice(), window.getSurface());
        auto capa = Info::GetSurfaceCapabilities(device.getPhysicalDevice(), window.getSurface());
        u32 minImageCount = getMinimumImageCount(device.getPhysicalDevice(), window.getSurface());
        vector<u32> queueIndices = {device.getGraphicsIndex()};

        auto createInfo = Info::SwapchainCreateInfo(m_Formats, window.getSurface(), minImageCount,
                                                    capa.currentTransform, queueIndices);

        vkCreateSwapchainKHR(device.get(), &createInfo, nullptr, &m_Swapchain);
        m_Images = CreateSwapchainImagesAndViews(device.get(), m_Swapchain, m_Formats.Format.format);
        printf("Swapchain: %lims\n", clock() - StartTime);
    }

    ws::Swapchain::Swapchain(ws::Device device, ws::Window window) {
        create(device, window);
    }

    VkSwapchainKHR ws::Swapchain::get() { return m_Swapchain; }

    VkExtent2D ws::Swapchain::getExtent() { return m_Formats.Extent; }

    SurfaceDisplayFormats ws::Swapchain::getFormats() { return m_Formats; }

    u32 ws::Swapchain::getImageCount() { return m_Images.Image.size(); }

    VkFramebuffer *ws::Swapchain::getFramebuffers() { return m_Framebuffers.data(); };

    void ws::Swapchain::initializeFramebuffers(ws::Device &device, VkRenderPass renderPass) {
        m_Framebuffers.resize(m_Images.Image.size());
        for (int i = 0; i < m_Framebuffers.size(); i++) {
            std::vector<VkImageView> attachments = {m_Images.View[i]};
            auto info = Info::FramebufferCreateInfo(renderPass, attachments, m_Formats.Extent, 1);
            vkCreateFramebuffer(device.get(), &info, nullptr, &m_Framebuffers[i]);
        }
    }

    u32 ws::Swapchain::getImageIndex(ws::Device device, VkSemaphore &semaphore, Result *result) {
        u32 index = 0;
        *result = vkAcquireNextImageKHR(device.get(), m_Swapchain, UINT64_MAX, semaphore, nullptr, &index);
        return index;
    }

    void ws::Swapchain::destroy(ws::Device device) {
        for (auto &i: m_Framebuffers)
            vkDestroyFramebuffer(device.get(), i, nullptr);
        for (auto &i: m_Images.View)
            vkDestroyImageView(device.get(), i, nullptr);
        vkDestroySwapchainKHR(device.get(), m_Swapchain, nullptr);
    }

    void ws::Swapchain::recreate(ws::Device device, ws::Window window) {
        create(device, window);
    }
}