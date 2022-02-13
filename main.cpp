


#include <iostream>
#include "Sound/audio_out.h" // sound
#include "Engine/Engine.hpp"
#include "lib/glm-master/glm/gtx/string_cast.hpp"



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
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texPos;
    static vector<VkVertexInputBindingDescription> getBindings() {
        vector<VkVertexInputBindingDescription> bindings;
        bindings.emplace_back(Info::VertexInputBindingDescription(0, sizeof(vertex), VK_VERTEX_INPUT_RATE_VERTEX));
        return bindings;
    }
    static vector<VkVertexInputAttributeDescription> getAttributes() {
        vector<VkVertexInputAttributeDescription> attribute;
        attribute.emplace_back(Info::VertexInputAttributeDescription(0, 0, offsetof(vertex, pos), SHADER_TYPE_vec3));
        attribute.emplace_back(Info::VertexInputAttributeDescription(0, 1, offsetof(vertex, color), SHADER_TYPE_vec3));
        attribute.emplace_back(Info::VertexInputAttributeDescription(0, 2, offsetof(vertex, texPos), SHADER_TYPE_vec2));

        return attribute;
    }
};

struct UBO {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

void RecreateSwapchain(ws::Device device, ws::Window window, ws::Swapchain& swapchain, ws::GraphicsPipeline& pipeline,
                       ws::RenderPass& renderPass, ws::CommandPool& pool, ws::CommandBuffer& commandBuffer,
                       ws::UniformBuffer& uniformBuffer, Shader vShader, Shader fShader,
                       VertexInputData inputData) {

    int width = 0, height = 0;
    glfwGetFramebufferSize(window.get(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window.get(), &width, &height);
        glfwWaitEvents();
    }

    device.waitIdle();
    commandBuffer.freeBuffers(device);
    swapchain.destroy(device);
    pipeline.destroy(device);
    renderPass.destroy(device);
    uniformBuffer.destroy(device);

    swapchain.recreate(device, window);
    renderPass.recreate(device, swapchain);
    uniformBuffer.recreate(device, pool.get(), sizeof(UBO), swapchain.getImageCount());
    pipeline.recreate(device, swapchain, renderPass, vShader, fShader,
                      inputData, uniformBuffer);


    VkRenderPass renderpass = renderPass.get();
    swapchain.initializeFramebuffers(device, renderpass);
    commandBuffer = pool.AllocateBuffers(device, swapchain.getImageCount());

}


struct point {
    glm::vec3 pos;
};
void pushRect(vector<point>& vec, float x, float y, float w, float h) {
    vec.push_back({{x, y, 0.0f}});
    vec.push_back({{x+w, y, 0.0f}});
    vec.push_back({{x+w, y+h, 0.0f}});
    vec.push_back({{x, y+h, 0.0f}});
}

int main() {



    const vector<uint16_t> indices = { 0,1,2,2,3,0 };
    vector<point> vertices = {
            {{-0.5f, -0.5f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}},
            {{-0.5f, 0.5f, 0.0f}}
    };

    vector<point> vertices2{};
    pushRect(vertices2, -1.0f, -1.0f, 1.0f, 1.0f);


    u32 currentVertices = 0;

    vector<VkVertexInputBindingDescription> Bindings {
            Info::VertexInputBindingDescription(0, sizeof(point), VK_VERTEX_INPUT_RATE_VERTEX)
    };
    vector<VkVertexInputAttributeDescription> Attributes {
            Info::VertexInputAttributeDescription(0, 0, offsetof(point, pos), SHADER_TYPE_vec3)
    };
    VertexInputData VertexInput {
            .attributes = Attributes,
            .bindings = Bindings,
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


    ws::UniformBuffer uniformBuffers(device, commandPool.get(), sizeof(UBO), swapchain.getImageCount());
    ws::GraphicsPipeline pipeline(device, swapchain, renderpass, vShader, fShader, VertexInput, uniformBuffers);

    
    for (u32 i = 0; i < swapchain.getImageCount(); i++) {
        commandBuffers.setIndex(i);
        commandBuffers.record(RenderPassObject, framebuffers[i], swapchain.getExtent());
        commandBuffers.bindPipeline(pipeline.get(), VK_PIPELINE_BIND_POINT_GRAPHICS);
        commandBuffers.bindVertexBuffer(vBuffer);
        commandBuffers.bindIndexBuffer(iBuffer);
        commandBuffers.bindDesciptorSets(pipeline.getLayout(), pipeline.getDescriptorSet()[i]);
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
        if (imageResult == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapchain(device, window, swapchain, pipeline, renderpass, commandPool, commandBuffers, uniformBuffers, vShader, fShader, VertexInput);
            SwapchainObject = swapchain.get();
            RenderPassObject = renderpass.get();
            for (u32 i = 0; i < swapchain.getImageCount(); i++) {
                commandBuffers.setIndex(i);
                commandBuffers.record(RenderPassObject, framebuffers[i], swapchain.getExtent());
                commandBuffers.bindPipeline(pipeline.get(), VK_PIPELINE_BIND_POINT_GRAPHICS);
                commandBuffers.bindVertexBuffer(vBuffer);
                commandBuffers.bindIndexBuffer(iBuffer);
                commandBuffers.bindDesciptorSets(pipeline.getLayout(), pipeline.getDescriptorSet()[i]);
                commandBuffers.drawIndexed(static_cast<u32>(indices.size()), 1, 0, 0, 0);
                commandBuffers.stopRecording();
            }
        }



        UBO uniformData;
        uniformData.model = glm::mat4(1.0f);
        uniformData.view = glm::mat4(1.0f);//glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        uniformData.proj = glm::mat4(1.0f);//glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 0.1f, 10.0f);
        uniformData.proj[1][1] *= -1;


        UPDATE_UNIFORM_BUFFER(device.get(), uniformBuffers, uniformData, imageIndex);

        device.resetFences(&inFlightFences[frame], 1);
        vector<VkPipelineStageFlags> waitFlags = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkCommandBuffer currentBuffer = commandBuffers.get(imageIndex);
        auto submitInfo = Info::SubmitInfo(imageAvailableSemaphores[frame], renderFinishedSemaphores[frame], currentBuffer, waitFlags);
        device.submitQueue(submitInfo, inFlightFences[frame]);
        auto presentInfo = Info::PresentInfo(SwapchainObject, renderFinishedSemaphores[frame], imageIndex);
        auto presentResult = device.presentQueue(presentInfo);
        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
            RecreateSwapchain(device, window, swapchain, pipeline, renderpass, commandPool, commandBuffers, uniformBuffers, vShader, fShader, VertexInput);
            SwapchainObject = swapchain.get();
            RenderPassObject = renderpass.get();
            for (u32 i = 0; i < swapchain.getImageCount(); i++) {
                commandBuffers.setIndex(i);
                commandBuffers.record(RenderPassObject, framebuffers[i], swapchain.getExtent());
                commandBuffers.bindPipeline(pipeline.get(), VK_PIPELINE_BIND_POINT_GRAPHICS);
                commandBuffers.bindVertexBuffer(vBuffer);
                commandBuffers.bindIndexBuffer(iBuffer);
                commandBuffers.bindDesciptorSets(pipeline.getLayout(), pipeline.getDescriptorSet()[i]);
                commandBuffers.drawIndexed(static_cast<u32>(indices.size()), 1, 0, 0, 0);
                commandBuffers.stopRecording();
            }
        }


        window.swapBuffers();
        frame++;
        if (frame > 1) frame = 0;
    }

    glfwTerminate();

}


