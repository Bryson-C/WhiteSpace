

#include "Engine/Engine.hpp"
#include <iostream>


class Timer {
private:
    bool setup;
    long time;
public:
    Timer() : setup(false), time(clock()) {};
    bool isComplete(int delay) { setup = false; return clock()-time>delay; };
    void reset() { time = clock(); };
    void set() { if (!setup) { time = clock(); setup = true; }};
};



int main() {
    $::Instance instance("Application", true);
    $::Window window(instance, 800, 600);
    $::Device device(instance, window);
    // TODO: Fix Swapchain, Something is off putting
    $::Swapchain swapchain(device, window);
    auto SwapchainObject = swapchain.get();
    $::CommandPool commandPool(device);
    auto commandBuffers = commandPool.AllocateBuffers(device, swapchain.getImageCount());
    $::RenderPass renderpass(device, swapchain);
    auto RenderPassObject = renderpass.get();
    swapchain.initializeFramebuffers(device, RenderPassObject);
    auto framebuffers = swapchain.getFramebuffers();

    Shader vShader = CreateShader(device.get(), R"(D:\graphics\graphics\vertex.spv)", VERTEX_SHADER);
    Shader fShader = CreateShader(device.get(), R"(D:\graphics\graphics\fragment.spv)", FRAGMENT_SHADER);

    $::GraphicsPipeline pipeline(device, swapchain, renderpass, vShader, fShader);

    for (u32 i = 0; i < swapchain.getImageCount(); i++) {
        commandBuffers.setIndex(i);
        commandBuffers.record(RenderPassObject, framebuffers[i], swapchain.getExtent());
        commandBuffers.bindPipeline(pipeline.get(), VK_PIPELINE_BIND_POINT_GRAPHICS);
        commandBuffers.draw(3, 1, 0, 0);
        commandBuffers.stopRecording();
    }

    u32 frame = 0;
    constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;
    vec<VkSemaphore> imageAvailableSemaphores = CreateSemaphore(device.get(), MAX_FRAMES_IN_FLIGHT);
    vec<VkSemaphore> renderFinishedSemaphores = CreateSemaphore(device.get(), MAX_FRAMES_IN_FLIGHT);
    vec<VkFence> inFlightFences = CreateFence(device.get(), MAX_FRAMES_IN_FLIGHT);


    while (window.isOpen()) {
        window.pollEvents();

        device.waitForFences(&inFlightFences[frame], 1);

        u32 imageIndex = 0;
        vkAcquireNextImageKHR(device.get(), swapchain.get(), UINT64_MAX, imageAvailableSemaphores[frame], nullptr, &imageIndex);


        device.resetFences(&inFlightFences[frame], 1);
        vec<VkPipelineStageFlags> waitFlags = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkCommandBuffer currentBuffer = commandBuffers.get(imageIndex);
        auto submitInfo = Info::SubmitInfo(imageAvailableSemaphores[frame], renderFinishedSemaphores[frame], currentBuffer, waitFlags);
        device.SubmitQueue(submitInfo, inFlightFences[frame]);
        auto presentInfo = Info::PresentInfo(SwapchainObject, renderFinishedSemaphores[frame], imageIndex);
        device.PresentQueue(presentInfo);


        window.swapBuffers();
        frame++;
        if (frame > 1) frame = 0;
    }

    glfwTerminate();

}
