//
// Created by Owner on 11/5/2021.
//

#ifndef GRAPHICS_ENGINE2_HPP
#define GRAPHICS_ENGINE2_HPP

/// TODO: Add Object Destruction Functions

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





/// TODO: Fix Settings System (Its A Mess)

/* Deprecated
struct Window {
    GLFWwindow* Window;
    VkSurfaceKHR Surface;
    bool isOpen() { return !glfwWindowShouldClose(Window); }
};
struct Instance {
    VkInstance Instance;
};
struct Device {
    VkPhysicalDevice PhysicalDevice;
    VkDevice Device;
};
struct CommandPool {
    VkCommandPool Pool;
    std::vector<VkCommandBuffer> Buffers{};
};
struct Descriptor {
    VkDescriptorPool Pool;
};

struct Swapchain {
    VkSwapchainKHR swapchain;
    SurfaceDisplayFormats formats;
    VkViewport viewport;
    VkRect2D scissor;
    std::vector<VkImage> images{};
    std::vector<VkImageView> imageViews{};
    std::vector<VkFramebuffer> framebuffers{};
};

struct Viewport {
    VkViewport Viewport;
    VkRect2D Scissor;
};
struct Pipeline {
    VkPipeline Pipeline;
    VkPipelineLayout Layout;
};
*/
template<typename T>
struct v2 {
    T x,y;
};

namespace $ {

    // Helper Objects
    struct ImageArray {
        vec<VkImage>     Image;
        vec<VkImageView> View;
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
        static void resizeCallbacK(GLFWwindow* window, int w, int h);
    public:
        Window($::Instance& instance, u32 width, u32 height);
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
        Device($::Instance& instance, Window& window);
        VkDevice get();
        VkPhysicalDevice getPhysicalDevice();
        u32 getGraphicsIndex();
        VkResult SubmitQueue(VkSubmitInfo& submitInfo, VkFence fence);
        VkResult PresentQueue(VkPresentInfoKHR& presentInfo);
        VkResult waitForFences(VkFence* fences, u32 count);
        VkResult resetFences(VkFence* fences, u32 count);
    };

    class Swapchain {
    private:
        VkSwapchainKHR m_Swapchain;
        SurfaceDisplayFormats m_Formats;
        $::ImageArray m_Images;
        vec<VkFramebuffer> m_Framebuffers;
    public:
        Swapchain($::Device& device, $::Window& window);
        Swapchain(SurfaceDisplayFormats formats, VkSwapchainKHR swapchain, $::ImageArray Images);
        VkSwapchainKHR get();
        VkExtent2D getExtent();
        u32 getImageCount();
        SurfaceDisplayFormats getFormats();
        VkFramebuffer* getFramebuffers();
        void initializeFramebuffers($::Device& device, VkRenderPass& renderPass);
    };

    class CommandBuffer {
    private:
        VkCommandPool* m_OwnerPool;
        vec<VkCommandBuffer> m_Buffers;
        u32 m_Index;
    public:
        CommandBuffer($::Device& device, VkCommandPool* owner, u32 count);
        void nextIndex();
        void prevIndex();
        void setIndex(u32 index);

        void record(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent);
        void stopRecording();
        void bindPipeline(VkPipeline pipeline, VkPipelineBindPoint bindPoint);
        void draw(u32 vertexCount, u32 instances, u32 firstVertex, u32 firstInstance);

        VkCommandBuffer get(u32 index);
    };

    class CommandPool {
    private:
        VkCommandPool m_Pool;
        vec<VkCommandBuffer> m_Buffers;
    public:
        CommandPool($::Device device);
        CommandBuffer AllocateBuffers($::Device& device, u32 count);
    };

    class RenderPass {
    private:
        VkRenderPass m_RenderPass;
    public:
        RenderPass($::Device& device, $::Swapchain& swapchain);
        VkRenderPass get();
    };

    class GraphicsPipeline {
    private:
        VkPipeline m_Pipeline;
        VkPipelineLayout m_Layout;
    public:
        GraphicsPipeline($::Device &device, $::Swapchain& swapchain, $::RenderPass& renderPass, Shader &vShader, Shader &fShader);
        VkPipeline get();
    };
}


enum class settings {
    Debug = 0,
    DontLogSuccess = 1,
    LogErrorCount = 2,
    NoWindowHeader = 3,
};
static std::vector<settings> gSettings{};
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


/*
namespace {
    inline bool GetSurfaceSupport(Device device, Window window) {
        VkBool32 support;
        vkGetPhysicalDeviceSurfaceSupportKHR(device.PhysicalDevice, device.Transfer.Index, window.Surface, &support);
        if (!support) vkGetPhysicalDeviceSurfaceSupportKHR(device.PhysicalDevice, device.Graphics.Index, window.Surface, &support); else return true;
        return support;
    }
    inline Result CreateSwapchainImagesAndViews(Device device, Swapchain &swapchain) {
        u32 imageCount = 0;
        vkGetSwapchainImagesKHR(device.Device, swapchain.swapchain, &imageCount, nullptr);
        swapchain.images.resize(imageCount);
        auto imgres = vkGetSwapchainImagesKHR(device.Device, swapchain.swapchain, &imageCount, swapchain.images.data());
        if (imgres != SUCCESS) return imgres;

        for (int i = 0; i < swapchain.images.size(); i++) {
            if (swapchain.images[i] == VK_NULL_HANDLE) continue;
            VkImageView view;
            auto range = Info::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
            auto info = Info::ImageViewCreateInfo(swapchain.images[i], VK_IMAGE_VIEW_TYPE_2D, swapchain.formats.Format.format, Info::ComponentMapping(), range);
            auto res = vkCreateImageView(device.Device, &info, nullptr, &view);
            if (res != SUCCESS) return res;
            swapchain.imageViews.push_back(view);
        }
        return SUCCESS;
    }
}
*/
Result Initalize();
VkInstance CreateInstance();

Result RecordCommandBuffer(VkCommandBuffer& Commandbuffer);
Result EndCommandBuffer(VkCommandBuffer& Commandbuffer);
//Result BeginRenderPass(Swapchain swapchain, VkRenderPass& renderPass, VkFramebuffer& Framebuffer, VkCommandBuffer& commandBuffer);
Result BeginRenderPass(VkExtent2D extent, VkRenderPass renderPass, VkFramebuffer& Framebuffer, VkCommandBuffer& commandBuffer);
Result EndRenderPass(VkCommandBuffer& Commandbuffer);
std::vector<VkSemaphore> CreateSemaphore(VkDevice device, const u32 count);
std::vector<VkFence> CreateFence(VkDevice device, const u32 count);
Shader CreateShader(VkDevice device, const std::string& path, VkShaderStageFlagBits type);

 /*
Result CreateWindow(Window& window, int w, int h, std::string name, Instance instance);
Window CreateWindow(int w, int h, std::string name, Instance instance);
Result CreateInstance(Instance& instance);


/// TODO: Fix Device Creation
Result CreateDevice(Instance instance, Device& device, Window window);
Device CreateDevice(Instance instance, Window window);


Result CreateDescriptor(Device device, Descriptor& descriptor, const std::vector<VkDescriptorPoolSize>& poolSize);
Result CreateSwapchain(Device device, Window window, Swapchain& swapchain);
Swapchain CreateSwapchain(Device device, Window window);

Result CreateShader(Device device, const std::string& path, Shader& shader, VkShaderStageFlagBits type);
Shader CreateShader(Device device, const std::string& path, VkShaderStageFlagBits type);

Result CreateRenderPass(Device device, const Swapchain& swapchain, VkRenderPass& renderPass);
VkRenderPass CreateRenderPass(Device device, Swapchain& swapchain);
Result CreatePipeline(Device device, const Swapchain& swapchain, VkRenderPass& renderPass, Shader vertex, Shader fragment, Pipeline& pipeline);
Pipeline CreatePipeline(Device device, Swapchain swapchain, VkRenderPass renderPass, Shader vertex, Shader fragment);
Result CreateFramebuffers(Device device, Swapchain& swapchain, VkRenderPass renderPass);
Result CreateCommandPool(Device device, Queue queue, CommandPool& pool);
CommandPool CreateCommandPool(Device device, Queue queue);
Result CreateCommandBuffer(Device device, CommandPool& pool, u32 count);
Result RecordCommandBuffer(VkCommandBuffer& Commandbuffer);
Result EndCommandBuffer(VkCommandBuffer& Commandbuffer);
Result BeginRenderPass(Swapchain swapchain, VkRenderPass& renderPass, VkFramebuffer& Framebuffer, VkCommandBuffer& commandBuffer);
Result BeginRenderPass(VkExtent2D extent, VkRenderPass& renderPass, VkFramebuffer& Framebuffer, VkCommandBuffer& commandBuffer);
Result EndRenderPass(VkCommandBuffer& Commandbuffer);
Result CreateSemaphore(Device device, std::vector<VkSemaphore>& semaphores, const u32 count);
std::vector<VkSemaphore> CreateSemaphore(Device device, const u32 count);
std::vector<VkSemaphore> CreateSemaphore(VkDevice device, const u32 count);
Result CreateFence(Device device, std::vector<VkFence>& fence, const u32 count);
std::vector<VkFence> CreateFence(Device device, const u32 count);
std::vector<VkFence> CreateFence(VkDevice device, const u32 count);
*/

$::Swapchain createSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow* window, u32 graphicsIndex, u32 presentIndex);


#endif //GRAPHICS_ENGINE2_HPP
