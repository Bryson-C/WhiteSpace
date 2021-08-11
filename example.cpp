#include "Engine/Engine.hpp"
// please dont use this. its not a good idea. I forgot to fix a memory leak imvolving command buffers and not freeing them ( or something like that )

void recreateSwapchain(WS::Devices Devices, WS::SurfaceObjects SurfaceObjects, WS::QueueList QueueList, WS::Queue& gQueue, WS::Queue& pQueue,
                        VkSwapchainKHR& Swapchain ,VkRenderPass& RenderPass, VkPipelineLayout& PipelineLayout, VkPipeline& GraphicsPipeline,
                         WS::Framebuffers& FrameBuffers, WS::CommandBuffers CommandBuffers,VkCommandPool& CommandPool, VkFormat& Format, VkExtent2D& Extent, WS::ImageList Images,
                         WS::ImageViews ImageViews
) {
    auto Device = Devices.Device;
    auto PhysicalDevice = Devices.PhysicalDevice;


    vkDeviceWaitIdle(Device);
    WS::resetCommandBuffers(Device, CommandPool);
    WS::destroyFrameBuffers(FrameBuffers, Device);
    WS::destroyPipeline(GraphicsPipeline, Device);
    WS::destroyRenderPass(RenderPass, Device);
    WS::destroyCommandBuffers(CommandPool, CommandBuffers, Device);
    WS::destroyCommandPool(CommandPool, Device);    
    WS::destroyImageViews(ImageViews, Device);
    WS::destroySwapchain(Swapchain, Device);
    
    


    Swapchain = WS::createSwapchain(Devices, SurfaceObjects, QueueList, Format, Extent, gQueue.Index, pQueue.Index);
    Images = WS::getSwapchainImages(Device, Swapchain);
    ImageViews = WS::createImageViews(Images, Device, Format);

    vkGetDeviceQueue(Device, gQueue.Index, 0, &gQueue.Queue);
    vkGetDeviceQueue(Device, pQueue.Index, 0, &pQueue.Queue);
    
    //auto DescriptorSetLayout = WS::createDescriptorSetLayout(Device);
    //auto DescriptorPool = WS::createDescriptorPool(Device, Images);
    //auto DescriptorSets = WS::createDescriptorSet(Device, DescriptorPool, DescriptorSetLayout, Images, );

    RenderPass = WS::createRenderPass(Device, Format);
    auto ShaderProgram = WS::createShaderProgram(Device, "C:\\Users\\Owner\\Documents\\code\\whitespace_test\\vertex.spv", "C:\\Users\\Owner\\Documents\\code\\whitespace_test\\fragment.spv");
    GraphicsPipeline = WS::createGraphicsPipeline(Devices, ShaderProgram, RenderPass, Extent, PipelineLayout, DRAWMODE_FILL);
    FrameBuffers = WS::createFrameBuffers(Device, ImageViews, RenderPass, Extent);
    CommandPool = WS::createCommandPool(Device, gQueue.Index, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}





int main() {


    //const std::vector<WS::vertex> vertices {
    //    {{-0.5f, 0.5f},  {1.0f,0.0f,0.0f}},
    //    {{0.0f, -0.5f},  {0.0f,1.0f,0.0f}},
    //    {{0.5f,  0.5f},  {0.0f,0.0f,1.0f}}
    //}
    const std::vector<WS::vertex> vertices {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    std::vector<uint16_t> indices {0,1,2,2,3,0};


    auto Instance = WS::createInstance(true);
    auto SurfaceObjects = WS::createWindow("Window", 800, 600, Instance);
    auto PhysicalDevice = WS::findPhysicalDevice(Instance);
    auto QueueList = WS::findQueues(PhysicalDevice);
    auto Device = WS::createDevice(PhysicalDevice, QueueList);
    WS::Devices Devices = {PhysicalDevice,Device};

    WS::Queue gQueue;
    WS::Queue pQueue;
   

    auto vBuffer = WS::createVertexBuffer(Devices, sizeof(vertices[0]) * vertices.size());
    auto iBuffer = WS::createIndexBuffer(Devices, sizeof(indices[0]) * indices.size(), indices);
    
    auto isBuffer = WS::createStagingBuffer(Devices, sizeof(indices[0]) * indices.size());
    auto vsBuffer = WS::createStagingBuffer(Devices, sizeof(vertices[0]) * vertices.size());
    WS::allocateBuffer(Device, vsBuffer, vertices.data(), sizeof(vertices[0]) * vertices.size());
    WS::allocateBuffer(Device, isBuffer, indices.data(), sizeof(indices[0]) * indices.size());

    
    VkFormat ImageFormat;
    VkExtent2D Extent;
    auto Swapchain = WS::createSwapchain(Devices, SurfaceObjects, QueueList, ImageFormat, Extent, gQueue.Index, pQueue.Index);
    auto Images = WS::getSwapchainImages(Device, Swapchain);
    auto ImageViews = WS::createImageViews(Images, Device, ImageFormat);

    vkGetDeviceQueue(Device, gQueue.Index, 0, &gQueue.Queue);
    vkGetDeviceQueue(Device, pQueue.Index, 0, &pQueue.Queue);
    
    //auto DescriptorSetLayout = WS::createDescriptorSetLayout(Device);
    //auto DescriptorPool = WS::createDescriptorPool(Device, Images);
    //auto DescriptorSets = WS::createDescriptorSet(Device, DescriptorPool, DescriptorSetLayout, Images, );

    auto RenderPass = WS::createRenderPass(Device, ImageFormat);
    auto ShaderProgram = WS::createShaderProgram(Device, "C:\\Users\\Owner\\Documents\\code\\whitespace_test\\vertex.spv", "C:\\Users\\Owner\\Documents\\code\\whitespace_test\\fragment.spv");
    VkPipelineLayout pipelineLayout {};
    auto GraphicsPipeline = WS::createGraphicsPipeline(Devices, ShaderProgram, RenderPass, Extent, pipelineLayout, DRAWMODE_FILL);
    auto FrameBuffers = WS::createFrameBuffers(Device, ImageViews, RenderPass, Extent);
    auto CommandPool = WS::createCommandPool(Device, gQueue.Index, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);


    WS::copyBuffer(Device, vsBuffer, vBuffer, sizeof(vertices[0]) * vertices.size(), CommandPool, gQueue.Queue);
    WS::copyBuffer(Device, isBuffer, iBuffer, sizeof(indices[0]) * indices.size(), CommandPool, gQueue.Queue);
    
    WS::destroyBuffer(isBuffer, Device);
    WS::destroyBuffer(vsBuffer, Device);
    //auto CommandBuffers = WS::createCommandBuffers(Device, CommandPool, FrameBuffers, RenderPass, GraphicsPipeline, Extent, vBuffer.Buffer, {}, {}, {}, &pipelineLayout);



    const int MAX_FRAMES_IN_FLIGHT = 2;
    auto ImageSemaphores     = WS::createSemaphores(MAX_FRAMES_IN_FLIGHT, Device);
    auto RenderSemaphores    = WS::createSemaphores(MAX_FRAMES_IN_FLIGHT, Device);
    auto Fences              = WS::createFences(MAX_FRAMES_IN_FLIGHT, Device);
    WS::FenceList ImageFence; ImageFence.resize(Images.size(), VK_NULL_HANDLE);   
    int currentFrame = 0;

    int fps = 0;
    int fpsTime = clock();
    while(WS::windowIsOpen(SurfaceObjects)) {
        auto presentTime = clock();
        glfwPollEvents();

        
        auto CommandBuffers = WS::startCmdBufferRecording(FrameBuffers, CommandPool, Device, RenderPass, Extent);
        for (int i = 0; i < FrameBuffers.size(); i++) {
            vkCmdBindPipeline(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);
            VkDeviceSize offsets[] {0};
            vkCmdBindVertexBuffers(CommandBuffers[i], 0, 1, &vBuffer.Buffer, offsets);
            vkCmdBindIndexBuffer(CommandBuffers[i], iBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);
            

            for (int j = 0; j < 4; j++) {
                WS::PushConstantData push{};
                push.offset = {0.0f, 0.2f * (j + 1)};
                push.color = {0.0f, 0.0f, 0.2f * (j+1)};

                vkCmdPushConstants(CommandBuffers[i], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(WS::PushConstantData), &push);
                vkCmdDrawIndexed(CommandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
                //vkCmdDraw(CommandBuffers[i], 3, 1, 0, 0);
            }
            vkCmdDrawIndexed(CommandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
            //vkCmdDraw(CommandBuffers[i], 3, 1, 0, 0);
        }
        WS::stopCmdBufferRecording(CommandBuffers);


        int w,h;
        glfwGetFramebufferSize(SurfaceObjects.Window,&w,&h);
        if (w <= 0 || h <= 0) {
            
        } else {
            vkWaitForFences(Device, 1, &Fences[currentFrame], VK_TRUE, UINT64_MAX);
            vkResetFences(Device, 1, &Fences[currentFrame]);
            

            
            uint32_t imageIndex = 0;
            VkResult imageResult = vkAcquireNextImageKHR(Device, Swapchain, UINT64_MAX, ImageSemaphores[currentFrame], nullptr, &imageIndex);

            if (imageResult == VK_ERROR_OUT_OF_DATE_KHR) {
                recreateSwapchain(Devices, SurfaceObjects, QueueList, gQueue, pQueue, Swapchain, RenderPass, pipelineLayout, GraphicsPipeline, FrameBuffers, CommandBuffers, CommandPool, ImageFormat, Extent, Images, ImageViews);
            }
            else if (imageResult != VK_SUCCESS && imageResult == VK_SUBOPTIMAL_KHR) {
                throw std::runtime_error("Failed To Get Swapchain Image");
            }



            VkSubmitInfo submitinfo{};
            VkSemaphore waitSemaphores[] = { ImageSemaphores[currentFrame] };
            VkSemaphore signalSemaphore[] = { RenderSemaphores[currentFrame] };
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            VkSwapchainKHR swapchains[] = { Swapchain };


            submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitinfo.waitSemaphoreCount = 1;
            submitinfo.pWaitSemaphores = waitSemaphores;
            submitinfo.pWaitDstStageMask = waitStages;
            submitinfo.commandBufferCount = 1;
            submitinfo.pCommandBuffers = &CommandBuffers[imageIndex];
            submitinfo.signalSemaphoreCount = 1;
            submitinfo.pSignalSemaphores = signalSemaphore;



            VkResult SubmitResult = vkQueueSubmit(gQueue.Queue, 1, &submitinfo, Fences[currentFrame]);
            if (SubmitResult != VK_SUCCESS) {
                WS::logError(SubmitResult, "Queue Submition: ");
                throw std::runtime_error("Failed To Submit Queue");
            }


            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphore;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapchains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr;

            VkResult presentResult = vkQueuePresentKHR(pQueue.Queue, &presentInfo);

            
            

            
            if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
                recreateSwapchain(Devices, SurfaceObjects, QueueList, gQueue, pQueue, Swapchain, RenderPass, pipelineLayout, GraphicsPipeline, FrameBuffers, CommandBuffers, CommandPool, ImageFormat, Extent, Images, ImageViews);
            }
            else if (presentResult != VK_SUCCESS ) {
                throw std::runtime_error("Failed To Get Swapchain Image");
            }
        }
        //WS::resetCommandBuffers(Device, CommandPool);


        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        
        fps++;
        if (clock()-fpsTime > 1000) {
            printf("FPS: %i\n", fps);
            fps = 0;
            fpsTime = clock();

            if ((clock()-presentTime) > 2) {
                printf("Present Time: %i\n", (clock()-presentTime));
            }
        }
        WS::swapWindowBuffers(SurfaceObjects);
    }



}
