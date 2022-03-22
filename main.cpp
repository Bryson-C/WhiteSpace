


#include <iostream>
#include "Sound/audio_out.h" // sound

#include "Engine/Infos/Infos.hpp"
#include "Engine/Instance/Instance.h"
#include "Engine/Device/Device.h"
#include "Engine/Swapchain/Swapchain.h"
#include "Engine/CommandPool/CommandPool.h"
#include "Engine/Buffers/Buffers.h"
#include "Engine/Pipeline/Pipeline.h"



#define GLM_FORCE_RADIANS
#include "lib/glm-master/glm/glm.hpp"
#include "lib/glm-master/glm/gtc/matrix_transform.hpp"
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
    UBO() {}
    UBO(glm::mat4 _model, glm::mat4 _view, glm::mat4 _proj) : model(_model), view(_view), proj(_proj) {}
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

void RecreateSwapchain(ws::Device device, ws::Window window, ws::Swapchain& swapchain, ws::GraphicsPipeline& pipeline,
                       std::vector<ws::UniformBuffer>& uniformBuffer, Shader vShader, Shader fShader,
                       VertexInputData inputData) {

    auto startTime = clock();

    int width = 0, height = 0;
    glfwGetFramebufferSize(window.get(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window.get(), &width, &height);
        glfwWaitEvents();
    }



    device.waitIdle();
    swapchain.destroy(device);
    pipeline.destroy(device);
    for (auto& ub : uniformBuffer) { ub.destroy(device); }
    uniformBuffer.clear();
    swapchain.recreate(device, window);

    uniformBuffer.resize(1);
    uniformBuffer[0].recreate(device,sizeof(UBO), swapchain.getImageCount());

    pipeline.recreate(device, swapchain, vShader, fShader, inputData, uniformBuffer);

    swapchain.initializeFramebuffers(device, pipeline.getRenderPass());

    printf("Recreation Of Graphics Objects: %ims\n", clock()-startTime);
}


struct point {
    glm::vec3 pos;
};

void pushRect(vector<point>& vec, float x, float y, float w, float h) {
    vec.push_back({{x,   y,   0.0f}});
    vec.push_back({{x+w, y,   0.0f}});
    vec.push_back({{x+w, y+h, 0.0f}});
    vec.push_back({{x,   y+h, 0.0f}});
}


struct pushData {
    float x, y;
};


class WSInstance {
private:
    // Objects

    ws::Instance        Instance;
    ws::Window          Window;
    ws::Device          Device;
    ws::Swapchain       Swapchain;
    VkSwapchainKHR      SwapchainObject;
    ws::CommandPool     CommandPool;
    ws::CommandBuffer   CommandBuffer;

    // Render Objects

    VkFramebuffer*                      Framebuffers;
    std::vector<ws::UniformBuffer>      UniformBuffers;
    ws::GraphicsPipeline                GraphicsPipeline;

    // Graphics Data
    Shader*             VShader;
    Shader*             FShader;
    VertexInputData*    InputData;
    std::vector<point>  Vertices;
    std::vector<u16>    Indices;
    ws::VertexBuffer    VertexBuffer;
    ws::IndexBuffer     IndexBuffer;
    VkBuffer            VBuffer;
    VkBuffer            IBuffer;

    // Render Sub-Objects
    u32 frame = 0;
    const u32                   MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<VkSemaphore>    imageAvailableSemaphores;
    std::vector<VkSemaphore>    renderFinishedSemaphores;
    std::vector<VkFence>        inFlightFences;
    u32 FPS = 0;
    u32 currentTime;

public:
    WSInstance(u32 width,
               u32 height) :
                    Instance("App", true),
                    Window(Instance, width, height),
                    Device(Instance, Window),
                    Swapchain(Device, Window),
                    SwapchainObject(Swapchain.get()),
                    CommandPool(Device)
                    {
                        CommandBuffer = CommandPool.AllocateBuffers(Device, Swapchain.getImageCount());
                        ws::UniformBuffer uniformBuffer(Device, sizeof(UBO), Swapchain.getImageCount());
                        UniformBuffers.push_back(uniformBuffer);

                        imageAvailableSemaphores = Device.createSemaphores(MAX_FRAMES_IN_FLIGHT);
                        renderFinishedSemaphores = Device.createSemaphores(MAX_FRAMES_IN_FLIGHT);
                        inFlightFences = Device.createFences(MAX_FRAMES_IN_FLIGHT);

                        currentTime = clock();
                    }

    void CreateGraphicsPipeline(Shader* vertexShader, Shader* fragmentShader, VertexInputData* inputData) {
        if (vertexShader) VShader = vertexShader;
        if (fragmentShader) FShader = fragmentShader;
        if (inputData) InputData = inputData;

        if (VShader && FShader && InputData)
            GraphicsPipeline.create(Device, Swapchain, *VShader, *FShader, *InputData, UniformBuffers);
        else
            throw std::runtime_error("Pipeline Data May be Invalid or Un-supplied Thereby Causing An Error");

        Swapchain.initializeFramebuffers(Device, GraphicsPipeline.getRenderPass());
        Framebuffers = Swapchain.getFramebuffers();
    }

    void UploadVertices(std::vector<point> vertices, std::vector<u16> indices) {
        Vertices = vertices;
        Indices = indices;
        VertexBuffer.create(Device, sizeof(vertices[0]) * vertices.size(), vertices.data());
        IndexBuffer.create(Device, sizeof(indices[0]) * indices.size(), indices.data());
        VBuffer = VertexBuffer.get();
        IBuffer = IndexBuffer.get();
    }

    void DrawFrame(pushData pData, UBO* uData) {

        // start drawing

        Window.pollEvents();
        Device.waitForFences(&inFlightFences[frame], 1);

        Result imageResult;
        auto imageIndex = Swapchain.getImageIndex(Device, imageAvailableSemaphores[frame], &imageResult);
        if (imageResult == VK_ERROR_OUT_OF_DATE_KHR) {
            //RecreateSwapchain(device, window, swapchain, pipeline, Uniforms, vShader, fShader, VertexInput);
            SwapchainObject = Swapchain.get();
        }

        CommandBuffer.setIndex(imageIndex);
        CommandBuffer.record(GraphicsPipeline.getRenderPass(), Framebuffers[imageIndex], Swapchain.getExtent());
        CommandBuffer.bindPipeline(GraphicsPipeline.get(), VK_PIPELINE_BIND_POINT_GRAPHICS);
        CommandBuffer.bindVertexBuffer(VBuffer);
        CommandBuffer.bindIndexBuffer(IBuffer);
        CommandBuffer.bindDesciptorSets(GraphicsPipeline.getLayout(), GraphicsPipeline.getDescriptorSet()[imageIndex]);
        CommandBuffer.bindPushConstants(GraphicsPipeline.getLayout(), VERTEX_SHADER, (void**) &pData, sizeof(pushData), 0);
        u32 instanceCount = 0;
        if (Vertices.size() % 4 == 0) instanceCount = Vertices.size() / 4;
        CommandBuffer.drawIndexed(static_cast<u32>(Indices.size()), instanceCount, 0, 0, 0);
        CommandBuffer.stopRecording();



        UBO uniformData(glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.5f));
        uniformData.proj[1][1] *= -1;
        if (uData) uniformData = *uData;
        UniformBuffers[0].update(Device, (void**) &uniformData, imageIndex);


        Device.resetFences(&inFlightFences[frame], 1);
        vector<VkPipelineStageFlags> waitFlags = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkCommandBuffer currentBuffer = CommandBuffer.get(imageIndex);

        auto submitInfo = Info::SubmitInfo(imageAvailableSemaphores[frame], renderFinishedSemaphores[frame], currentBuffer, waitFlags);
        Device.submitQueue(submitInfo, inFlightFences[frame]);

        auto presentInfo = Info::PresentInfo(SwapchainObject, renderFinishedSemaphores[frame], imageIndex);
        auto presentResult = Device.presentQueue(presentInfo);

        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
            //RecreateSwapchain(Device, Window, Swapchain, GraphicsPipeline, UniformBuffers, *VShader, *FShader, *InputData);
            SwapchainObject = Swapchain.get();
        }


        Window.swapBuffers();
        frame++;
        FPS++;
        if (frame > 1) frame = 0;
        if (currentTime + 1000 < clock()) {
            currentTime = clock();
            printf("FPS: %i\n", FPS);
            FPS = 0;
        }
    }

    Shader CreateShader(std::string path, VkShaderStageFlagBits type) { return ::CreateShader(Device.get(), path, type); }

    ws::CommandBuffer* getCommandBuffers() { return &CommandBuffer; }

    bool running() { return Window.isOpen(); }
};


int main() {
    const vector<uint16_t> indices = { 0,1,2,2,3,0 };
    vector<point> vertices = {};
    pushRect(vertices, -0.5f, -0.5f, 1.0f, 1.0f);

    vector<VkVertexInputBindingDescription> Bindings {
            Info::VertexInputBindingDescription(0, sizeof(point), VK_VERTEX_INPUT_RATE_VERTEX)
    };
    vector<VkVertexInputAttributeDescription> Attributes {
            Info::VertexInputAttributeDescription(0, 0, offsetof(point, pos), SHADER_TYPE_vec3)
    };
    vector<VkPushConstantRange> PushConstants {
            Info::PushConstantRange(VERTEX_SHADER, sizeof(pushData), 0)
    };
    VkPushConstantRange info;
    info.size = 0;
    info.offset = 0;
    info.stageFlags = 0;
    VertexInputData VertexInput {
            .attributes = Attributes,
            .bindings = Bindings,
            .pushConstants = &PushConstants
    };



    static WSInstance Instance(800, 600);
    Shader vShader = Instance.CreateShader( R"(D:\graphics\graphics\vertex.spv)", VERTEX_SHADER);
    Shader fShader = Instance.CreateShader( R"(D:\graphics\graphics\fragment.spv)", FRAGMENT_SHADER);
    Instance.UploadVertices(vertices, indices);
    Instance.CreateGraphicsPipeline(&vShader, &fShader, &VertexInput);
    while (Instance.running()) {
        Instance.DrawFrame({0.0f,0.0f}, nullptr);
    }

    return 0;
}


int main2() {



    const vector<uint16_t> indices = { 0,1,2,2,3,0 };
    vector<point> vertices = {};
    pushRect(vertices, -0.5f, -0.5f, 1.0f, 1.0f);


    vector<VkVertexInputBindingDescription> Bindings {
        Info::VertexInputBindingDescription(0, sizeof(point), VK_VERTEX_INPUT_RATE_VERTEX)
    };
    vector<VkVertexInputAttributeDescription> Attributes {
        Info::VertexInputAttributeDescription(0, 0, offsetof(point, pos), SHADER_TYPE_vec3)
    };
    vector<VkPushConstantRange> PushConstants {
        Info::PushConstantRange(VERTEX_SHADER, sizeof(pushData), 0)
    };
    VkPushConstantRange info;
    info.size = 0;
    info.offset = 0;
    info.stageFlags = 0;
    VertexInputData VertexInput {
            .attributes = Attributes,
            .bindings = Bindings,
            .pushConstants = &PushConstants
    };

    ws::Instance instance("Application", true);
    ws::Window window(instance, 800, 600);
    ws::Device device(instance, window);
    ws::Swapchain swapchain(device, window);
    auto SwapchainObject = swapchain.get();
    ws::CommandPool commandPool(device);
    auto commandBuffers = commandPool.AllocateBuffers(device, swapchain.getImageCount());

    ws::VertexBuffer vertexBuffer(device, sizeof(vertices[0]) * vertices.size(), vertices.data());
    auto vBuffer = vertexBuffer.get();
    ws::IndexBuffer indexBuffer(device, sizeof(indices[0]) * indices.size(), (void*) indices.data());
    auto iBuffer = indexBuffer.get();

    Shader vShader = CreateShader(device.get(), R"(D:\graphics\graphics\vertex.spv)", VERTEX_SHADER);
    Shader fShader = CreateShader(device.get(), R"(D:\graphics\graphics\fragment.spv)", FRAGMENT_SHADER);



    ws::UniformBuffer uniformBuffers(device, sizeof(UBO), swapchain.getImageCount());
    std::vector<ws::UniformBuffer> Uniforms = { uniformBuffers };

    ws::GraphicsPipeline pipeline(device, swapchain, vShader, fShader, VertexInput, Uniforms);
    swapchain.initializeFramebuffers(device, pipeline.getRenderPass());
    auto framebuffers = swapchain.getFramebuffers();


    u32 frame = 0;
    constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;
    auto imageAvailableSemaphores = device.createSemaphores(MAX_FRAMES_IN_FLIGHT);
    auto renderFinishedSemaphores = device.createSemaphores(MAX_FRAMES_IN_FLIGHT);
    auto inFlightFences = device.createFences(MAX_FRAMES_IN_FLIGHT);

    auto currentTime = clock();
    u32 FPS = 0;

    pushData lastData{};

    while (window.isOpen()) {

        window.pollEvents();
        device.waitForFences(&inFlightFences[frame], 1);

        Result imageResult;
        auto imageIndex = swapchain.getImageIndex(device, imageAvailableSemaphores[frame], &imageResult);
        if (imageResult == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapchain(device, window, swapchain, pipeline, Uniforms, vShader, fShader, VertexInput);
            SwapchainObject = swapchain.get();
        }


        commandBuffers.setIndex(imageIndex);
        commandBuffers.record(pipeline.getRenderPass(), framebuffers[imageIndex], swapchain.getExtent());
        commandBuffers.bindPipeline(pipeline.get(), VK_PIPELINE_BIND_POINT_GRAPHICS);
        commandBuffers.bindVertexBuffer(vBuffer);
        commandBuffers.bindIndexBuffer(iBuffer);
        commandBuffers.bindDesciptorSets(pipeline.getLayout(), pipeline.getDescriptorSet()[imageIndex]);
        pushData pData{};
        pData = {.x = 0.0f, .y = 0.0f};
        lastData = pData;
        commandBuffers.bindPushConstants(pipeline.getLayout(), VERTEX_SHADER, (void**) &pData, sizeof(pushData), 0);
        u32 instanceCount = 0;
        if (vertices.size() % 4 == 0) instanceCount = vertices.size() / 4;
        commandBuffers.drawIndexed(static_cast<u32>(indices.size()), instanceCount, 0, 0, 0);
        commandBuffers.stopRecording();



        UBO uniformData(glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.5f));
        uniformData.proj[1][1] *= -1;
        Uniforms[0].update(device, (void**) &uniformData, imageIndex);


        device.resetFences(&inFlightFences[frame], 1);
        vector<VkPipelineStageFlags> waitFlags = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkCommandBuffer currentBuffer = commandBuffers.get(imageIndex);

        auto submitInfo = Info::SubmitInfo(imageAvailableSemaphores[frame], renderFinishedSemaphores[frame], currentBuffer, waitFlags);
        device.submitQueue(submitInfo, inFlightFences[frame]);

        auto presentInfo = Info::PresentInfo(SwapchainObject, renderFinishedSemaphores[frame], imageIndex);
        auto presentResult = device.presentQueue(presentInfo);

        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
            RecreateSwapchain(device, window, swapchain, pipeline, Uniforms, vShader, fShader, VertexInput);
            SwapchainObject = swapchain.get();
        }


        window.swapBuffers();
        frame++;
        FPS++;
        if (frame > 1) frame = 0;
        if (currentTime + 1000 < clock()) {
            currentTime = clock();
            printf("FPS: %i\n", FPS);
            FPS = 0;
        }
    }

    glfwTerminate();
    return 0;
}


