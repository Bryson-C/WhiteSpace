#include "Engine/Engine.hpp"


int main() {


    const std::vector<WS::vertex> vertices {
        {{-0.5f,0.0f}, {0.0f,0.0f,0.0f}},
        {{0.0f,-0.5f}, {0.0f,0.0f,0.0f}},
        {{0.5f,0.0f}, {0.0f,0.0f,0.0f}}
    };


    auto Instance = WS::createInstance(true);
    auto SurfaceObjects = WS::createWindow("Window", 800, 600, Instance);
    auto PhysicalDevice = WS::findPhysicalDevice(Instance);
    auto QueueList = WS::findQueues(PhysicalDevice);
    auto Device = WS::createDevice(PhysicalDevice, QueueList);
    WS::Devices Devices = {PhysicalDevice,Device};

    WS::Queue gQueue;
    WS::Queue pQueue;
   
    auto vBuffer = WS::createVertexBuffer(Devices, sizeof(vertices[0]) * vertices.size());
    auto sBuffer = WS::createStagingBuffer(Devices, sizeof(vertices[0]) * vertices.size());
    WS::allocateBuffer(Device, sBuffer, vertices.data(), sizeof(vertices[0]) * vertices.size());
    

    
    VkFormat ImageFormat;
    VkExtent2D Extent;
    auto Swapchain = WS::createSwapchain(Devices, SurfaceObjects, QueueList, ImageFormat, Extent, gQueue.Index, pQueue.Index);
    auto Images = WS::getSwapchainImages(Device, Swapchain);
    auto ImageViews = WS::createImageViews(Images, Device, ImageFormat);

    vkGetDeviceQueue(Device, gQueue.Index, 0, &gQueue.Queue);
    vkGetDeviceQueue(Device, pQueue.Index, 0, &pQueue.Queue);
    
    auto RenderPass = WS::createRenderPass(Device, ImageFormat);
    auto vShader = WS::createShaderModule("C:\\Users\\Owner\\Documents\\code\\whitespace_test\\vert.vert.spv", Device);
    auto fShader = WS::createShaderModule("C:\\Users\\Owner\\Documents\\code\\whitespace_test\\frag.frag.spv", Device);
    WS::ShaderProgram ShaderProgram = { vShader, fShader };
    auto GraphicsPipeline = WS::createGraphicsPipeline(Devices, ShaderProgram, RenderPass, Extent, DRAWMODE_FILL);
    auto FrameBuffers = WS::createFrameBuffers(Device, ImageViews, RenderPass, Extent);
    auto CommandPool = WS::createCommandPool(Device, gQueue.Index);

    WS::copyBuffer(Device, sBuffer, vBuffer, sizeof(vertices[0]) * vertices.size(), CommandPool, gQueue.Queue);


    auto CommandBuffers = WS::createCommandBuffers(Device, CommandPool, FrameBuffers, RenderPass, GraphicsPipeline, Extent, std::get<0>(vBuffer));



    const int MAX_FRAMES_IN_FLIGHT = 2;
    auto ImageSemaphores     = WS::createSemaphores(MAX_FRAMES_IN_FLIGHT, Device);
    auto RenderSemaphores    = WS::createSemaphores(MAX_FRAMES_IN_FLIGHT, Device);
    auto Fences              = WS::createFences(MAX_FRAMES_IN_FLIGHT, Device);
    WS::FenceList ImageFence; ImageFence.resize(Images.size(), VK_NULL_HANDLE);   
    int currentFrame = 0;

    while(WS::windowIsOpen(SurfaceObjects)) {
        glfwPollEvents();


        int w,h;
        glfwGetFramebufferSize(std::get<0>(SurfaceObjects),&w,&h);
        if (w <= 0 || h <= 0) {
            
        } else {
            vkWaitForFences(Device, 1, &Fences[currentFrame], VK_TRUE, UINT64_MAX);
            vkResetFences(Device, 1, &Fences[currentFrame]);

            uint32_t imageIndex = 0;
            VkResult imageResult = vkAcquireNextImageKHR(Device, Swapchain, UINT64_MAX, ImageSemaphores[currentFrame], nullptr, &imageIndex);

            if (imageResult == VK_ERROR_OUT_OF_DATE_KHR) {
                //WS::recreateSwapchain(Device, PhysicalDevice, dir("shaders\\defaultVertex.spv").c_str(), dir("shaders\\defaultFragment.spv").c_str(), SurfaceObjects, 
                //queueList, FrameBuffers, CommandPool, CommandBuffers, GraphicsPipeline, RenderPass, Swapchain, SwapchainImageViews, DescriptorSet, {VertexBuffer,IndexBuffer,indices}, UniformObject, DescriptorPool, DrawMode);
            }
            else if (imageResult != VK_SUCCESS && imageResult == VK_SUBOPTIMAL_KHR) {
                throw std::runtime_error("Failed To Get Swapchain Image");
            }
            //if (enableUniformBuffers) {
            //    updateUniformBuffers(imageIndex);
            //}
            //if () {
            //    updateUniformBuffers(Device, UniformBuffers, imageIndex, SwapchainExtent);
            //}
            // I get errors from code below
            //if (Images[imageIndex] != VK_NULL_HANDLE) {
            //    vkWaitForFences(Device, 1, &Images[imageIndex], VK_TRUE, UINT64_MAX);
            //}
            //Images[imageIndex] = Fences[currentFrame];



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
                //recreateSwapchain();
            }
            else if (presentResult != VK_SUCCESS ) {
                throw std::runtime_error("Failed To Get Swapchain Image");
            }
        }







        WS::swapWindowBuffers(SurfaceObjects);
    }





}