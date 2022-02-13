//
// Created by Owner on 11/5/2021.
//

#ifndef GRAPHICS_ENGINE2_HPP
#define GRAPHICS_ENGINE2_HPP

/// TODO: Add Object Destruction Functions
/// TODO: Fix Settings System (Its A Mess)
/// TODO: Add Safety Features In Case Of API Failures
/// TODO: Please For The Love Of God Clean Up The Code
/// TODO: Fix Useless References

#define VKPATH "../lib/VKInclude/vulkan/vulkan.h"
#define VKHPPPATH "../lib/VKInclude/vulkan/vulkan.h"
#define GLFWPATH "../lib/GLFW/include/glfw3.h"

#include VKPATH
#include VKHPPPATH
#include GLFWPATH
#define GLM_FORCE_RADIANS
#include "../lib/glm-master/glm/glm.hpp"
#include "../lib/glm-master/glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <array>
#include <optional>
#include <algorithm>
#include "Infos/Infos.hpp"





template<typename T>
struct v2 {
    T x,y;
};



enum class settings {
    Debug = 0,
    DontLogSuccess = 1,
    LogErrorCount = 2,
    NoWindowHeader = 3,
};
static vector<settings> gSettings{};
inline void addSetting(settings name) { gSettings.emplace_back(name); }
inline bool settingDefined(settings name) { for (auto i : gSettings) { if (i == name) return true; } return false; }

inline void LogError(VkResult result, const char* Object = "") {
    if (settingDefined(settings::DontLogSuccess) && result == SUCCESS) return;
    printf(Object);
    switch (result) {
        case VK_SUCCESS:
            printf("Success\n");
            break;
        case VK_NOT_READY:
            printf("Not Ready\n");
            break;
        case VK_TIMEOUT:
            printf("Timeout\n");
            break;
        case VK_EVENT_SET:
            printf("Event Set\n");
            break;
        case VK_EVENT_RESET:
            printf("Event Reset\n");
            break;
        case VK_INCOMPLETE:
            printf("Incomplete\n");
            break;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            printf("Host Ran Out Of Memory\n");
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            printf("Device Ran Out Of Memory\n");
            break;
        case VK_ERROR_INITIALIZATION_FAILED:
            printf("Initialization Failed\n");
            break;
        case VK_ERROR_DEVICE_LOST:
            printf("Device Lost\n");
            break;
        case VK_ERROR_MEMORY_MAP_FAILED:
            printf("Memory Map Failed\n");
            break;
        case VK_ERROR_LAYER_NOT_PRESENT:
            printf("Layer Not Present\n");
            break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            printf("Extension Not Present\n");
            break;
        case VK_ERROR_FEATURE_NOT_PRESENT:
            printf("Feature Not Present\n");
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            printf("Incompatible Driver\n");
            break;
        case VK_ERROR_TOO_MANY_OBJECTS:
            printf("Too Many Objects\n");
            break;
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            printf("Format Not Supported\n");
            break;
        case VK_ERROR_FRAGMENTED_POOL:
            printf("Fragmented Pool\n");
            break;
        case VK_ERROR_UNKNOWN:
            printf("Error Unknown\n");
            break;
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            printf("Pool Ran Out Of Memory\n");
            break;
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            printf("Invalid External Handle\n");
            break;
        case VK_ERROR_FRAGMENTATION:
            printf("Fragmentation Error\n");
            break;
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
            printf("Invalid Opaque Capture Address\n");
            break;
        case VK_ERROR_SURFACE_LOST_KHR:
            printf("Surface Lost\n");
            break;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            printf("Native Window In Use\n");
            break;
        case VK_SUBOPTIMAL_KHR:
            printf("Suboptimal[KHR]\n");
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            printf("Out Of Date[KHR]\n");
            break;
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            printf("Incompatible Display[KHR]\n");
            break;
        case VK_ERROR_VALIDATION_FAILED_EXT:
            printf("Validation Failed[EXT]\n");
            break;
        case VK_ERROR_INVALID_SHADER_NV:
            printf("Invalid Shader[NV]\n");
            break;
            //case VK_ERROR_INCOMPATIBLE_VERSION_KHR: R.I.P. Not in new version of Vulkan (but it did do its job)
            //	printf("Incompatible Version[KHR]\n");
            //	break;
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
            printf("Invalid DRM Format Modifier Plane Layout[EXT]");
            break;
        case VK_ERROR_NOT_PERMITTED_EXT:
            printf("Not Permitted Error[EXT]\n");
            break;
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
            printf("Full Screen Exclusive Mode Lost[EXT]\n");
            break;
        case VK_THREAD_IDLE_KHR:
            printf("Thread Idle[KHR]\n");
            break;
        case VK_THREAD_DONE_KHR:
            printf("Thread Done[KHR]\n");
            break;
        case VK_OPERATION_DEFERRED_KHR:
            printf("Operation Deferred[KHR]\n");
            break;
        case VK_OPERATION_NOT_DEFERRED_KHR:
            printf("Operation Not Deferred[KHR]\n");
            break;
        case VK_PIPELINE_COMPILE_REQUIRED_EXT:
            printf("Pipeline Compile Required[EXT]\n");
            break;
        case VK_RESULT_MAX_ENUM:
            printf("Max Enum\n");
            break;
        default:
            break;
    }
}



namespace Info {
    namespace Debug {
        inline static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
        inline VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT callback);
        inline Result CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    }
}


Result Initalize();
VkInstance CreateInstance();

Result RecordCommandBuffer(VkCommandBuffer& Commandbuffer);
Result EndCommandBuffer(VkCommandBuffer& Commandbuffer);
Result BeginRenderPass(VkExtent2D extent, VkRenderPass renderPass, VkFramebuffer& Framebuffer, VkCommandBuffer& commandBuffer);
Result EndRenderPass(VkCommandBuffer& Commandbuffer);

Shader CreateShader(VkDevice device, const std::string& path, VkShaderStageFlagBits type);
BufferObject CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlag);
void CopyBuffer(VkDevice device, VkCommandPool pool, VkQueue queue, VkBuffer dst, VkBuffer src, VkDeviceSize size);
#define CREATE_STAGING_BUFFER(device, physicalDevice, size) \
    CreateBuffer(device, physicalDevice, size, BUFFER_SENDER, RAM_MEMORY | COHERENT_MEMORY)

namespace ws {

    struct PipelineObject {
        VkPipelineLayout Layout;
        VkPipeline Pipeline;
    };

    struct DescriptorObject {
        VkDescriptorSetLayout Layout;
        VkDescriptorPool Pool;
        vector<VkDescriptorSet> Sets;
    };
    struct WriteInfo {
        u32 binding;
        std::optional<VkDescriptorImageInfo> imageInfo{};
        std::optional<VkDescriptorBufferInfo> bufferInfo{};
    };
    struct DescriptorSet {
        VkShaderStageFlags Access;
        VkDescriptorType Type;
        uint32_t Count;
        std::vector<WriteInfo> DescriptorWrite;
    };






#define UPDATE_UNIFORM_BUFFER(device, uniformBuffers, uniformData, imageIndex) \
    auto m_Memory = uniformBuffers.getMem();               \
    void* UNIFORM_UPLOAD; \
    vkMapMemory(device, m_Memory[imageIndex], 0, uniformBuffers.getSize(), 0, &UNIFORM_UPLOAD); \
    memcpy(UNIFORM_UPLOAD, &uniformData, uniformBuffers.getSize()); \
    vkUnmapMemory(device, m_Memory[imageIndex])

#define UPLOAD_BUFFER_TO_MEMORY(device, memory, bufferdata, size) \
    void* data;                             \
    vkMapMemory(device, memory, 0, size, 0, &data); \
    memcpy(data, bufferdata, (size_t) size); \
    vkUnmapMemory(device, memory)




    // Helper Objects
    struct ImageArray {
        vector<VkImage>     Image;
        vector<VkImageView> View;
    };
    struct UniformDescription {};
    struct Texture {
        std::string handle;
        VkImage Image;
        VkImageView View;
        VkDeviceMemory Memory;
    };

    // Object Wrappers

    class Instance {
    private:
        VkInstance m_Instance;
        const char *m_InstanceName;
        bool m_Debug;
    public:
        Instance(const char* name = EngineName, bool debug = false);
        VkInstance get();
        const char* getName();
    };

    class Window {
    private:
        GLFWwindow* m_Window;
        VkSurfaceKHR m_Surface;
    public:
        Window(ws::Instance& instance, u32 width, u32 height);
        bool isOpen();
        void swapBuffers();
        void pollEvents();
        GLFWwindow* get();
        VkSurfaceKHR getSurface();
        VkExtent2D size() const;
        v2<u32> getMousePos();
        bool keyDown(int keycode);
    };

    class Device {
    private:
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        u32 m_GraphicsIndex, m_PresentIndex;
        VkQueue m_GraphicsQueue, m_PresentQueue;
    public:
        Device(ws::Instance& instance, Window& window);
        VkDevice get();
        VkPhysicalDevice getPhysicalDevice();
        u32 getGraphicsIndex();
        VkQueue getGraphicsQueue();
        VkResult submitQueue(VkSubmitInfo& submitInfo, VkFence fence);
        VkResult presentQueue(VkPresentInfoKHR& presentInfo);
        VkResult waitForFences(VkFence* fences, u32 count);
        VkResult resetFences(VkFence* fences, u32 count);
        vector<VkSemaphore> createSemaphores(u32 count);
        vector<VkFence> createFences(u32 count);
        void waitIdle();
    };

    // Soon to be parent object of vertex and index buffers
    class Buffer {
    protected:
        VkBuffer m_Buffer;
        VkDeviceMemory m_Memory;
    public:
        Buffer();
        Buffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, VkBufferUsageFlags bufferType, void* bufferdata);
        void destroy(ws::Device device);
        VkBuffer get();
    };

    class VertexBuffer  {
        VkBuffer m_Buffer, m_StagingBuffer;
        VkDeviceMemory m_Memory, m_StagingMemory;
    public:
        VertexBuffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, void* vertexdata);
        void upload(ws::Device device, VkCommandPool pool, VkDeviceSize size, void* vertexdata);
        VkBuffer get();
        void destroy(ws::Device device);
    };

    class IndexBuffer : public Buffer {
    public:
        IndexBuffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, void* indexdata);
    };

    class UniformBuffer {
    private:
        vector<VkBuffer> m_Buffer;
        vector<VkDeviceMemory> m_Memory;
        VkDeviceSize m_Size;
        void create(ws::Device device, VkCommandPool pool, VkDeviceSize size, u32 count = 1);
    public:
        UniformBuffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, u32 count = 1);
        void update(ws::Device device, void* data, u32 index);
        VkBuffer* get();
        VkDeviceSize getSize();
        vector<VkDeviceMemory> getMem();
        void destroy(ws::Device device);
        void recreate(ws::Device device, VkCommandPool pool, VkDeviceSize size, u32 count = 1);
    };



    class CommandBuffer {
    private:
        VkCommandPool* m_OwnerPool;
        vector<VkCommandBuffer> m_Buffers;
        u32 m_Index;
    public:
        CommandBuffer(ws::Device& device, VkCommandPool* owner, u32 count);
        void nextIndex();
        void prevIndex();
        void setIndex(u32 index);

        void record(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent);
        void stopRecording();
        void bindPipeline(VkPipeline pipeline, VkPipelineBindPoint bindPoint);
        void draw(u32 vertexCount, u32 instances, u32 firstVertex, u32 firstInstance);
        void drawIndexed(u32 indexCount, u32 instances, u32 firstIndex, u32 firstVertex, u32 firstInstance);
        void bindVertexBuffer(VkBuffer buffer);
        void bindIndexBuffer(VkBuffer buffer, VkIndexType type = VK_INDEX_TYPE_UINT16);
        void bindDesciptorSets(VkPipelineLayout layout, VkDescriptorSet set);
        VkCommandBuffer get(u32 index);

        void freeBuffers(ws::Device device);
    };

    class CommandPool {
    private:
        VkCommandPool m_Pool;
        CommandBuffer* m_Buffers;
    public:
        CommandPool(ws::Device device);
        CommandBuffer AllocateBuffers(ws::Device& device, u32 count);
        VkCommandPool get();
        void destroy(ws::Device device);
    };

    class Swapchain {
    private:
        VkSwapchainKHR m_Swapchain;
        SurfaceDisplayFormats m_Formats;
        ws::ImageArray m_Images;
        vector<VkFramebuffer> m_Framebuffers;
        void create(ws::Device device, ws::Window window);
    public:
        Swapchain(ws::Device device, ws::Window window);
        VkSwapchainKHR get();
        VkExtent2D getExtent();
        u32 getImageCount();
        SurfaceDisplayFormats getFormats();
        VkFramebuffer* getFramebuffers();
        void initializeFramebuffers(ws::Device& device, VkRenderPass& renderPass);
        u32 getImageIndex(ws::Device device, VkSemaphore& semaphore, Result* result = nullptr);

        void recreate(ws::Device device, ws::Window window);
        void destroy(ws::Device device);
    };


    class RenderPass {
    private:
        VkRenderPass m_RenderPass;
        void create(ws::Device device, ws::Swapchain swapchain);
    public:
        RenderPass(ws::Device device, ws::Swapchain swapchain);
        VkRenderPass get() const;
        void recreate(ws::Device device, ws::Swapchain swapchain);
        void destroy(ws::Device device);
    };

    class Descriptor {
    private:
        VkDescriptorSetLayout m_Layout;
        vector<VkDescriptorPoolSize> m_Sizes;
        VkDescriptorPool m_Pool;
        vector<VkDescriptorSet> m_Sets;
    public:
        void createLayout(ws::Device device, vector<VkDescriptorSetLayoutBinding> bindings);

        void createPool(ws::Device device, const u32 descriptorCount[]);
        // Allocates A Set Amount Of Descriptor Sets
        void allocate(ws::Device device, u32 count);
        // Creates A Single Descriptor Set
        //void create();
        // Destroys A Single Descriptor Set
        //void destroy();
        // Frees The Entire Pool
        //void free();
    };


    class PipelineFactory {
    private:
        PipelineObject m_Pipelines{};
        std::vector<Shader> m_ShaderStages{};
        VkPipelineVertexInputStateCreateInfo m_VertexInfo{};
        VkPipelineInputAssemblyStateCreateInfo m_InputAssembly{};
        VkViewport m_Viewport;
        VkRect2D m_Scissor;
        VkPipelineRasterizationStateCreateInfo m_Rasterization{};
        VkPipelineMultisampleStateCreateInfo m_Multisample{};
        vector<VkPipelineColorBlendAttachmentState> colorBlendAttachment;
        vector<VkPushConstantRange> m_PushConstants{};
        DescriptorObject m_Descriptor{};
    public:
        // Adds A Shader To Be Processed Later During [build] Function
        void addShaderComponent(Shader shader);
        void addShaderComponent(std::initializer_list<Shader> shaders);

        // Adds A VertexInputState To Be Processed Later During [build] Function
        void addVertexInputComponent(VertexInputData vertexData);

        // Adds A InputAssemblyState To Be Processed Later During [build] Function
        void addInputAssemblyComponent(VkPrimitiveTopology topology);

        //Adds A ViewportState To Be Processed Later During [build] Function
        void addViewportComponent(VkExtent2D extent);
        void addViewportComponent(Window window);

        //Adds A RasterizationState To Be Processed Later During [build] Function
        void addRasterizationComponent(VkPolygonMode drawMode,
                                       VkCullModeFlags cullmode = VK_CULL_MODE_BACK_BIT,
                                       VkFrontFace frontface = VK_FRONT_FACE_COUNTER_CLOCKWISE);

        //Adds A MultisampleState To Be Processed Later During [build] Function
        void addMultiSampleComponent(VkSampleCountFlagBits samples);

        //Adds A ColorBlendState To Be Processed Later During [build] Function
        void addColorBlendComponent(vector<VkPipelineColorBlendAttachmentState> attachments = {});

        //Adds A Set Amount Of PushConstantRanges To Be Processed Later During [build] Function
        void addPushConstants();

        //Adds A Set Amount Of Descriptor Objects To Be Processed Later During [build] Function
        DescriptorObject* addDescriptorObjects(vector<ws::DescriptorSet> bindings, ws::Device device);

        //WIP
        void addSampler();

        // Final Processing Of The Pipeline, Returns A Pipeline Structure Containing Vital Information
        PipelineObject* build(ws::Device device, ws::RenderPass renderPass);
    };






// I made this simply because I dont want to keep writing it
#define GraphicsPipelineConstructor \
                        ws::Device device, ws::Swapchain swapchain, ws::RenderPass renderPass, \
                        Shader vShader, Shader fShader, VertexInputData inputData, \
                        ws::UniformBuffer& uniformBuffers

    class GraphicsPipeline {
    private:
        void create(GraphicsPipelineConstructor);
        VkPipeline m_Pipeline;
        VkPipelineLayout m_Layout;

        VkDescriptorSetLayout m_DescriptorLayout;
        VkDescriptorPool m_Pool;
        vector<VkDescriptorSet> m_DescriptorSets;
        void createDescriptor(ws::Device device, ws::UniformBuffer& buffers, u32 count);

        VkSampler m_Sampler;
        void createSampler(ws::Device device);
    public:
        //GraphicsPipeline() {};
        // Creates Graphics Pipeline And Related Objects
        GraphicsPipeline(GraphicsPipelineConstructor);
        // Returns Pipeline Object
        VkPipeline get();
        // Returns Pipeline Layout Object
        VkPipelineLayout getLayout();
        // Returns List Of Descriptor Sets
        vector<VkDescriptorSet> getDescriptorSet();
        void expectTextures(u32 count);
        void loadTexture(ws::Texture texture, const char* name);
        void unloadTexture(const char* name);

        //void newDescriptor(); // TODO: add a feature to manually add a new descriptor

        void destroy(ws::Device device);
        void recreate(GraphicsPipelineConstructor);
    };

    // Helper Functions


    /// TODO: Convert NDC To Screen Coords
#define scastF static_cast<float>
    inline glm::vec2 toScreenCoords(VkExtent2D extent, float x, float y);

    /// TODO: Image/Textures
    ws::Texture createTexture(ws::Device device, ws::CommandPool pool, const char* path);
}

inline void UNO_FLIP_FUCKING_SUCKS() {
    printf("Uno flip is literally the worst uno. you can not prove me otherwise\n"
           "In Uno Flip I dont feel like I loose my human rights everytime I play on the normal side and that is terrible\n"
           "I really do miss picking up more cards");
}


#endif //GRAPHICS_ENGINE2_HPP
