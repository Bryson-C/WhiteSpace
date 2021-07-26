#include "../lib/VKInclude/vulkan/vulkan.h"
#include "../lib/GLFW/include/glfw3.h"
#include "../lib/glm-master/glm/glm.hpp"
#include <iostream>
#include <vector>
#include <tuple>
#include <optional>
#include <array>
#include <algorithm>
#include <fstream>
#include <array>


#define DRAWMODE_LINES      VK_POLYGON_MODE_LINE
#define DRAWMODE_FILL       VK_POLYGON_MODE_FILL
#define DRAWMODE_POINTS     VK_POLYGON_MODE_POINT


namespace WS {

    struct Queue {
        VkQueue Queue;
        uint32_t Index; 
        VkQueueFlags Flag; 
    };
    struct vertex {
        glm::vec2 pos;
        glm::vec3 color;
        
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }
        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(vertex, color);

            return attributeDescriptions;
        }
    };
    struct uniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };


    typedef std::vector<WS::Queue>                                          QueueList;
    typedef std::vector<VkImage>                                            ImageList;
    typedef std::vector<VkImageView>                                        ImageViews;
    typedef std::vector<VkFramebuffer>                                      Framebuffers;
    typedef std::vector<VkCommandBuffer>                                    CommandBuffers;
    typedef std::vector<const char*>                                        Extensions;
    typedef std::tuple<GLFWwindow*, VkSurfaceKHR>                           SurfaceObjects;
    typedef std::tuple<VkSurfaceFormatKHR, VkPresentModeKHR, VkExtent2D>    PresentationInfo; 
    typedef std::tuple<VkPhysicalDevice,VkDevice>                           Devices;
    typedef std::array<VkShaderModule,2>                                    ShaderProgram;
    typedef std::vector<VkDescriptorSet>                                    DescriptorSets;
    typedef std::tuple<VkBuffer,VkDeviceMemory>                             BufferObjects;
    typedef std::tuple<VkBuffer,std::vector<uint16_t>>                      IndexBufferObjects;
    typedef std::vector<VkSemaphore>                                        SemaphoreList;
    typedef std::vector<VkFence>                                            FenceList;


    // Creation
    VkInstance              createInstance(bool Debug);
    VkPhysicalDevice        findPhysicalDevice(VkInstance Instance);
    QueueList               findQueues(VkPhysicalDevice Device);
    VkDevice                createDevice(VkPhysicalDevice PhysicalDevice, WS::QueueList& QueueList);
    SurfaceObjects          createWindow(const char* Title,int Width, int Height, VkInstance Instance);
    VkSwapchainKHR          createSwapchain(WS::Devices Devices, WS::SurfaceObjects SurfaceObjects, WS::QueueList QueueList, VkFormat& ImageFormat, VkExtent2D& Extent, uint32_t& GraphicsQueue, uint32_t& PresentQueue);
    ImageList               getSwapchainImages(VkDevice Device, VkSwapchainKHR Swapchain);
    ImageViews              createImageViews(WS::ImageList images, VkDevice device, VkFormat imageFormat);
    VkShaderModule          createShaderModule(const char* filename, VkDevice device);
    VkDescriptorPool        createDescriptorPool(VkDevice device, std::vector<VkImage>swapchainImages);
    VkDescriptorSetLayout   createDescriptorSetLayout(VkDevice device);
    DescriptorSets          createDescriptorSet(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, ImageList swapchainImages, std::vector<VkBuffer> uniformBuffers);
    VkPipeline              createGraphicsPipeline(WS::Devices Devices, WS::ShaderProgram Program, VkRenderPass RenderPass, VkExtent2D SwapchainExtent, VkPolygonMode DrawMode = VK_POLYGON_MODE_FILL, VkDescriptorSetLayout* DescriptorSet = nullptr);
    VkRenderPass            createRenderPass(VkDevice Device, VkFormat ImageFormat);
    Framebuffers            createFrameBuffers(VkDevice device, WS::ImageViews imageViews,VkRenderPass renderPass, VkExtent2D extent);
    VkCommandPool           createCommandPool(VkDevice Device, uint32_t queue);
    CommandBuffers          createCommandBuffers(VkDevice device, VkCommandPool commandPool, WS::Framebuffers framebuffers, VkRenderPass renderPass, VkPipeline graphicsPipeline, VkExtent2D extent, 
                                                VkBuffer vertexBuffer = VK_NULL_HANDLE, WS::IndexBufferObjects IndexBufferObjects = {}, WS::DescriptorSets descriptorSet = {}, VkPipelineLayout* pipelineLayout = nullptr);
    VkBuffer                createBuffer(WS::Devices Devices, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkDeviceMemory& BufferMemory, VkDeviceSize Size);
    BufferObjects           createStagingBuffer(WS::Devices Devices, VkDeviceSize SizeOfData);
    BufferObjects           createVertexBuffer(WS::Devices Devices, VkDeviceSize SizeOfData);
    SemaphoreList           createSemaphores(const size_t semaphoreCount, VkDevice device);
    FenceList               createFences(const size_t fenceCount, VkDevice device);


    // Other
    void                    logError(VkResult result, const char* Object);
    inline bool             windowIsOpen(SurfaceObjects SurfaceObjects) { return !glfwWindowShouldClose(std::get<0>(SurfaceObjects)); }
    inline bool             windowIsOpen(GLFWwindow* Window) { return !glfwWindowShouldClose(Window); }

    inline void             setWindowTitle(SurfaceObjects SurfaceObjects, const char* Title) { glfwSetWindowTitle(std::get<0>(SurfaceObjects), Title); }
    inline void             setWindowTitle(GLFWwindow* Window, const char* Title) { glfwSetWindowTitle(Window, Title); }
    inline void             swapWindowBuffers(SurfaceObjects SurfaceObjects) { glfwSwapBuffers(std::get<0>(SurfaceObjects)); }
    inline void             swapWindowBuffers(GLFWwindow* Window) { glfwSwapBuffers(Window); }

    uint32_t                findMemoryType(VkPhysicalDevice PhysicalDevice, uint32_t TypeFilter, VkMemoryPropertyFlags Properties);
    void                    allocateBuffer(VkDevice Device, VkDeviceMemory& DeviceMemory, const void* Data, VkDeviceSize Size);
    void                    allocateBuffer(VkDevice Device, BufferObjects& BufferObjects, const void* Data, VkDeviceSize Size);
    void                    copyBuffer(VkDevice Device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize Size, VkCommandPool CommandPool, VkQueue Queue);
    void                    copyBuffer(VkDevice Device, BufferObjects srcBuffer, BufferObjects dstBuffer, VkDeviceSize Size, VkCommandPool CommandPool, VkQueue Queue);
    
    
    //void                 recordCommandBuffers();
    //void                 uploadBuffer();



    // destroying objects
    inline void             destroyObjects(VkInstance instances) { vkDestroyInstance(instances, nullptr); }
    inline void             destroyObjects(VkDevice device) { vkDestroyDevice(device, nullptr); }
    inline void             destroyObjects(std::tuple<GLFWwindow*, VkSurfaceKHR> SurfaceObjects, VkInstance instance) { vkDestroySurfaceKHR(instance, std::get<1>(SurfaceObjects), nullptr); glfwDestroyWindow(std::get<0>(SurfaceObjects)); } 
    inline void             destroyObjects(VkSwapchainKHR swapchain,std::vector<VkImageView> imageViews, VkDevice device) { for (auto i : imageViews) { vkDestroyImageView(device, i, nullptr); } vkDestroySwapchainKHR(device, swapchain, nullptr); }
    inline void             destroyObjects(VkPipeline pipeline, VkDevice device) { vkDestroyPipeline(device, pipeline, nullptr); }
    inline void             destroyObjects(VkRenderPass renderPass, VkDevice device) { vkDestroyRenderPass(device, renderPass, nullptr); }
    inline void             destroyObjects(std::vector<VkFramebuffer> frameBuffers, VkCommandPool commandPool, std::vector<VkCommandBuffer> commandBuffers, VkDevice device) { for (auto i : frameBuffers) { vkDestroyFramebuffer(device, i, nullptr); } vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data()); }
    inline void             destroyObjects(VkCommandPool commandPool, VkDevice device) { vkDestroyCommandPool(device, commandPool, nullptr); }
    inline void             destroyObjects(std::vector<VkSemaphore> semaphores, VkDevice device) { for (auto i : semaphores) { vkDestroySemaphore(device, i, nullptr); }}
    inline void             destroyObjects(std::vector<VkFence> fences, VkDevice device) { for (auto i : fences) { vkWaitForFences(device, 1, &i, VK_TRUE, UINT64_MAX); vkDestroyFence(device, i, nullptr); }}
    inline void             destroyObjects(VkBuffer buffer, VkDeviceMemory deviceMemory, VkDevice device) { vkDestroyBuffer(device, buffer, nullptr); vkFreeMemory(device, deviceMemory, nullptr); }
    inline void             destroyObjects(BufferObjects BufferObjects, VkDevice device) { vkDestroyBuffer(device, std::get<0>(BufferObjects), nullptr); vkFreeMemory(device, std::get<1>(BufferObjects), nullptr); }
    inline void             destroyObjects(VkDescriptorSetLayout descriptorSet, VkDevice device) { vkDestroyDescriptorSetLayout(device, descriptorSet, nullptr);}
}

namespace WSDefaultCreation {

    VkApplicationInfo       ApplicationInfo();
    /*
        typedef struct VkApplicationInfo {
            VkStructureType    sType;
            const void*        pNext;
            const char*        pApplicationName;
            uint32_t           applicationVersion;
            const char*        pEngineName;
            uint32_t           engineVersion;
            uint32_t           apiVersion;
        } VkApplicationInfo;
    */                          
   VkDeviceCreateInfo       DeviceInfo(WS::QueueList queueList, WS::Extensions Extensions);
   /* 
        typedef struct VkDeviceCreateInfo {
            VkStructureType                    sType;
            const void*                        pNext;
            VkDeviceCreateFlags                flags;
            uint32_t                           queueCreateInfoCount;
            const VkDeviceQueueCreateInfo*     pQueueCreateInfos;
            uint32_t                           enabledLayerCount;
            const char* const*                 ppEnabledLayerNames;
            uint32_t                           enabledExtensionCount;
            const char* const*                 ppEnabledExtensionNames;
            const VkPhysicalDeviceFeatures*    pEnabledFeatures;
        } VkDeviceCreateInfo;
   */

}