//
// Created by Owner on 2/18/2022.
//

#ifndef ENGINE_SWAPCHAIN_H
#define ENGINE_SWAPCHAIN_H

#include "../../VKInclude/vulkan/vulkan.h"
#include "../Infos/Infos.hpp"
#include "../Device/Device.h"
#include "../Window/Window.h"
#include <algorithm>

namespace ws {
    class Swapchain {
    private:
        VkSwapchainKHR m_Swapchain;
        SurfaceDisplayFormats m_Formats;
        ImageArray m_Images;
        vector <VkFramebuffer> m_Framebuffers;

        void create(ws::Device device, ws::Window window);

    public:
        Swapchain(ws::Device device, ws::Window window);

        VkSwapchainKHR get();

        VkExtent2D getExtent();

        u32 getImageCount();

        SurfaceDisplayFormats getFormats();

        VkFramebuffer *getFramebuffers();

        void initializeFramebuffers(ws::Device &device, VkRenderPass renderPass);

        u32 getImageIndex(ws::Device device, VkSemaphore &semaphore, Result *result = nullptr);

        void recreate(ws::Device device, ws::Window window);

        void destroy(ws::Device device);
    };
}

#endif //ENGINE_SWAPCHAIN_H
