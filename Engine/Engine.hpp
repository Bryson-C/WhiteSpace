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
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <array>
#include <optional>
#include <algorithm>
#include VKPATH
#include VKHPPPATH
#include GLFWPATH
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

namespace ws {



    // Helper Objects
    struct ImageArray {
        vector<VkImage>     Image;
        vector<VkImageView> View;
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
        v2<u32> size() const;
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

    class VertexBuffer : public Buffer {
    public:
        VertexBuffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, void* vertexdata);
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
    public:
        UniformBuffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, u32 count = 1);
        void update(ws::Device device, void* data, u32 index);
        VkBuffer* get();
        VkDeviceSize getSize();
        vector<VkDeviceMemory> getMem();
        void destroy(ws::Device device);
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
        void create(ws::Device& device, ws::Window& window);
        void destroyFramebuffers(ws::Device device);
        void destroyImageViews(ws::Device device);
        void destroySwapchain(ws::Device device);
    public:
        Swapchain(ws::Device& device, ws::Window& window);
        VkSwapchainKHR get();
        VkExtent2D getExtent();
        u32 getImageCount();
        SurfaceDisplayFormats getFormats();
        VkFramebuffer* getFramebuffers();
        void initializeFramebuffers(ws::Device& device, VkRenderPass& renderPass);
        u32 getImageIndex(ws::Device device, VkSemaphore& semaphore, Result* result = nullptr);
    };


    class RenderPass {
    private:
        VkRenderPass m_RenderPass;
    public:
        RenderPass(ws::Device& device, ws::Swapchain& swapchain);
        VkRenderPass get() const;
    };



    class GraphicsPipeline {
    private:
        VkPipeline m_Pipeline;
        VkPipelineLayout m_Layout;

        VkDescriptorSetLayout m_DescriptorLayout;
        VkDescriptorPool m_Pool;
        vector<VkDescriptorSet> m_DescriptorSets;
        void createDescriptor(ws::Device device, ws::UniformBuffer& buffers, u32 count);
    public:
        GraphicsPipeline(ws::Device device, ws::Swapchain swapchain, ws::RenderPass renderPass, Shader vShader, Shader fShader);
        GraphicsPipeline(ws::Device device, ws::Swapchain swapchain, ws::RenderPass renderPass, Shader vShader, Shader fShader,
                         vector<VkVertexInputAttributeDescription>& attributes, vector<VkVertexInputBindingDescription>& bindings,
                         ws::UniformBuffer& uniformBuffers);
        VkPipeline get();
        VkPipelineLayout getLayout();
        vector<VkDescriptorSet> getDescriptorSet();
    };



    // Helper Functions
}


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



#endif //GRAPHICS_ENGINE2_HPP
