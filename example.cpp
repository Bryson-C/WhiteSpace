


#include "Engine/Engine.hpp"
#include <iostream>
#define GLM_FORCE_RADIANS
#include "lib/glm-master/glm/glm.hpp"
#include "lib/glm-master/glm/gtc/matrix_transform.hpp"
#include "Sound/audio_out.h" // sound

#undef CreateSemaphore

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

struct UniformObject {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);
};

struct vertex {
    glm::vec2 pos;
    glm::vec3 color;
    static vector<VkVertexInputBindingDescription> getBindings() {
        vector<VkVertexInputBindingDescription> bindings;
        VkVertexInputBindingDescription binding{};
        binding.binding = 0;
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        binding.stride = sizeof(vertex);
        bindings.push_back(binding);
        return bindings;
    }
    static vector<VkVertexInputAttributeDescription> getAttributes() {
        vector<VkVertexInputAttributeDescription> attribute;
        VkVertexInputAttributeDescription attrib[2]{};
        attrib[0].binding = 0;
        attrib[0].location = 0;
        attrib[0].format = SHADER_TYPE_vec2;
        attrib[0].offset = offsetof(vertex, pos);

        attrib[1].binding = 0;
        attrib[1].location = 1;
        attrib[1].format = SHADER_TYPE_vec3;
        attrib[1].offset = offsetof(vertex, color);
        attribute.push_back(attrib[0]);
        attribute.push_back(attrib[1]);
        return attribute;
    }
};

struct UBO {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

int main() {
    const vector<uint16_t> indices = { 0,1,2,2,3,0 };
    vector<vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    ws::Instance instance("Application", true);
    ws::Window window(instance, 800, 600);
    ws::Device device(instance, window);
    ws::Swapchain swapchain(device, window);
    auto SwapchainObject = swapchain.get();
    ws::CommandPool commandPool(device);
    auto commandBuffers = commandPool.AllocateBuffers(device, swapchain.getImageCount());
    ws::RenderPass renderpass(device, swapchain);
    auto RenderPassObject = renderpass.get();
    swapchain.initializeFramebuffers(device, RenderPassObject);
    auto framebuffers = swapchain.getFramebuffers();

    ws::VertexBuffer vertexBuffer(device, commandPool.get(), sizeof(vertices[0]) * vertices.size(), vertices.data());
    auto vBuffer = vertexBuffer.get();
    ws::IndexBuffer indexBuffer(device, commandPool.get(), sizeof(indices[0]) * indices.size(), (void*) indices.data());
    auto iBuffer = indexBuffer.get();

    Shader vShader = CreateShader(device.get(), R"(D:\graphics\graphics\vertex.spv)", VERTEX_SHADER);
    Shader fShader = CreateShader(device.get(), R"(D:\graphics\graphics\fragment.spv)", FRAGMENT_SHADER);

    auto attribs = vertex::getAttributes();
    auto bindings = vertex::getBindings();

    ws::UniformBuffer uniformBuffers(device, commandPool.get(), sizeof(UBO), swapchain.getImageCount());
    ws::GraphicsPipeline pipeline(device, swapchain, renderpass, vShader, fShader, attribs, bindings, uniformBuffers);


    for (u32 i = 0; i < swapchain.getImageCount(); i++) {
        commandBuffers.setIndex(i);
        commandBuffers.record(RenderPassObject, framebuffers[i], swapchain.getExtent());
        commandBuffers.bindPipeline(pipeline.get(), VK_PIPELINE_BIND_POINT_GRAPHICS);
        commandBuffers.bindVertexBuffer(vBuffer);
        commandBuffers.bindIndexBuffer(iBuffer);
        commandBuffers.bindDesciptorSets(pipeline.getLayout(), pipeline.getDescriptorSet()[i]);
        //commandBuffers.draw(static_cast<u32>(vertices.size()), 1, 0, 0);
        commandBuffers.drawIndexed(static_cast<u32>(indices.size()), 1, 0, 0, 0);
        commandBuffers.stopRecording();
    }

    u32 frame = 0;
    constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;
    auto imageAvailableSemaphores = device.createSemaphores(MAX_FRAMES_IN_FLIGHT);
    auto renderFinishedSemaphores = device.createSemaphores(MAX_FRAMES_IN_FLIGHT);
    auto inFlightFences = device.createFences(MAX_FRAMES_IN_FLIGHT);


    while (window.isOpen()) {
        window.pollEvents();

        device.waitForFences(&inFlightFences[frame], 1);

        Result imageResult;
        auto imageIndex = swapchain.getImageIndex(device, imageAvailableSemaphores[frame], &imageResult);
        if (imageResult == VK_ERROR_OUT_OF_DATE_KHR) {} // recreate swapchain

        UBO uniformData;
        uniformData.model = glm::rotate(glm::mat4(1.0f), 45.0f, glm::vec3(0.0f,0.0f,1.0f));
        uniformData.view = glm::mat4(1.0f);
        uniformData.proj = glm::mat4(1.0f);
        uniformData.proj[1][1] *= -1;

        //uniformBuffers.update(device, &uniformData, imageIndex);
        auto m_Memory = uniformBuffers.getMem();
                void* upload;
        vkMapMemory(device.get(), m_Memory[imageIndex], 0, uniformBuffers.getSize(), 0, &upload);
        memcpy(upload, &uniformData, uniformBuffers.getSize());
        vkUnmapMemory(device.get(), m_Memory[imageIndex]);

        device.resetFences(&inFlightFences[frame], 1);
        vector<VkPipelineStageFlags> waitFlags = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkCommandBuffer currentBuffer = commandBuffers.get(imageIndex);
        auto submitInfo = Info::SubmitInfo(imageAvailableSemaphores[frame], renderFinishedSemaphores[frame], currentBuffer, waitFlags);
        device.submitQueue(submitInfo, inFlightFences[frame]);
        auto presentInfo = Info::PresentInfo(SwapchainObject, renderFinishedSemaphores[frame], imageIndex);
        auto presentResult = device.presentQueue(presentInfo);
        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {} // recreate swapchain


        window.swapBuffers();
        frame++;
        if (frame > 1) frame = 0;
    }
    glfwTerminate();
}
