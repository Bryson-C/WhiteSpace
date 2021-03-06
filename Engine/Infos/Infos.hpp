//
// Created by Owner on 1/3/2022.
//

#ifndef ENGINE_INFOS_HPP
#define ENGINE_INFOS_HPP

/// TODO: Fix Infos To Use Default Vulkan Objects

#include "../../lib/VKInclude/vulkan/vulkan.h"
#include "../../lib/GLFW/include/glfw3.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <optional>
#include <functional>

// Preprocessor Defines
#define EngineName "WhiteSpace"
#define WindowObject GLFWwindow*
using std::vector;
#define u32 uint32_t
#define u16 uint16_t
#define PipelineAccess VkShaderStageFlags
#define Result VkResult
#define SUCCESS VK_SUCCESS
#define ERROR VK_ERROR_UNKNOWN
#define qGRAPHICS_FLAG VK_QUEUE_GRAPHICS_BIT
#define qTRANSFER_FLAG VK_QUEUE_TRANSFER_BIT
#define qCOMPUTE_FLAG VK_QUEUE_COMPUTE_BIT
#define VERTEX_SHADER VK_SHADER_STAGE_VERTEX_BIT
#define FRAGMENT_SHADER VK_SHADER_STAGE_FRAGMENT_BIT
#define BLEND_ONE VK_BLEND_FACTOR_ONE
#define BLEND_ZERO VK_BLEND_FACTOR_ZERO

#define TOPOLOGY_POINT_LIST VK_PRIMITIVE_TOPOLOGY_POINT_LIST
#define TOPOLOGY_LINE_LIST VK_PRIMITIVE_TOPOLOGY_LINE_LIST
#define TOPOLOGY_LINE_STRIP VK_PRIMITIVE_TOPOLOGY_LINE_STRIP
#define TOPOLOGY_TRIANGLE_LIST VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
#define TOPOLOGY_TRIANGLE_STRIP VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
#define TOPOLOGY_TRIANGLE_FAN VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN


#define SHADER_TYPE_float VK_FORMAT_R32_SFLOAT
#define SHADER_TYPE_vec2 VK_FORMAT_R32G32_SFLOAT
#define SHADER_TYPE_vec3 VK_FORMAT_R32G32B32_SFLOAT
#define SHADER_TYPE_vec4 VK_FORMAT_R32G32B32A32_SFLOAT

#define BUFFER_SENDER VK_BUFFER_USAGE_TRANSFER_SRC_BIT
#define BUFFER_RECIEVER VK_BUFFER_USAGE_TRANSFER_DST_BIT

#define COHERENT_MEMORY VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
#define RAM_MEMORY VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
#define GPU_MEMORY VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT

#define SAMPLER_REPEAT VK_SAMPLER_ADDRESS_MODE_REPEAT
#define FILTER_LINEAR VK_FILTER_LINEAR

#define UPLOAD_BUFFER_TO_MEMORY(device, memory, bufferdata, size)  \
    void* data;                                                     \
    vkMapMemory(device, memory, 0, size, 0, &data);                 \
    memcpy(data, bufferdata, (size_t) size);                        \
    vkUnmapMemory(device, memory)

#define CREATE_STAGING_BUFFER(device, physicalDevice, size) \
    CreateBuffer(device, physicalDevice, size, BUFFER_SENDER, RAM_MEMORY | COHERENT_MEMORY)


struct SurfaceDisplayFormats {
    VkSurfaceFormatKHR Format;
    VkPresentModeKHR PresentMode;
    VkExtent2D Extent;
};
struct Shader {
    VkShaderModule Module;
    VkShaderStageFlagBits Type;
};
struct BufferObject {
    VkBuffer Buffer;
    VkDeviceMemory Memory;
};
struct VertexInputData {
    vector<VkVertexInputAttributeDescription>& attributes;
    vector<VkVertexInputBindingDescription>& bindings;
    vector<VkPushConstantRange>* pushConstants = nullptr;
};
template<typename T>
struct v2 {
    T x,y;
};
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
struct PipelineObject {
    VkPipelineLayout Layout;
    VkPipeline Pipeline;
};

struct [[deprecated("Trying To Remove, Unsure Of Dependencies")]] WriteInfo {
    u32 binding;
    std::optional<VkDescriptorImageInfo> imageInfo{};
    std::optional<VkDescriptorBufferInfo> bufferInfo{};
};

struct [[deprecated("Trying To Remove, Unsure Of Dependencies")]] DescriptorSetInfo {
    VkShaderStageFlags Access;
    VkDescriptorType Type;
    uint32_t Count;
    std::vector<WriteInfo> DescriptorWrite;
};

#define DONT_LOG_SUCCESS true
inline void LogError(VkResult result, const char* Object = "") {
    if (DONT_LOG_SUCCESS && result == SUCCESS) return;
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


u32 findMemoryType(VkPhysicalDevice physicalDevice, u32 typeFilter, VkMemoryPropertyFlags properties);
void CopyBuffer(VkDevice device, VkCommandPool pool, VkQueue queue, VkBuffer dst, VkBuffer src, VkDeviceSize size);
Result Initalize();


VkInstance CreateInstance(bool debug = false);

Result RecordCommandBuffer(
        VkCommandBuffer &Commandbuffer);

Result EndCommandBuffer(
        VkCommandBuffer &Commandbuffer);

Result BeginRenderPass(
        VkExtent2D          extent,
        VkRenderPass        renderPass,
        VkFramebuffer       &Framebuffer,
        VkCommandBuffer     &commandBuffer);

Result EndRenderPass(
        VkCommandBuffer &Commandbuffer);

Shader CreateShader(
    VkDevice                device,
    const std::string       &path,
    VkShaderStageFlagBits   type);


namespace Info {
    // Instance

    VkApplicationInfo ApplicationCreateInfo(
            const char*                         appName,
            uint32_t                            appVersion,
            const char*                         engineName = EngineName,
            u32                                 engineVersion = 1);

    VkInstanceCreateInfo InstanceCreateInfo(
            const                               VkApplicationInfo appInfo,
            const std::vector<const char *>     &layers,
            const std::vector<const char *>     &extensions);

    // Device

    VkDeviceQueueCreateInfo QueueCreateInfo(
            uint32_t                            familyIndex,
            uint32_t                            count,
            const float                         *priority);
    VkDeviceCreateInfo DeviceCreateInfo(
            const std::vector<VkDeviceQueueCreateInfo>  &queues,
            const std::vector<const char *>             &layers,
            const std::vector<const char *>             &extensions,
            VkPhysicalDeviceFeatures                    *features = nullptr);


    VkCommandPoolCreateInfo CommandPoolCreateInfo(
            u32                                     QueueIndex,
            u32                                     flags = 0);

    VkCommandBufferAllocateInfo CommandBufferAllocationInfo(
            VkCommandPool                                       pool,
            u32                                                 bufferCount,
            VkCommandBufferLevel                                level);

    VkCommandBufferBeginInfo CommandBufferBeginInfo(
            VkCommandBufferUsageFlags       flags = 0);


    VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo(
            uint32_t                                        maxSets,
            std::vector<VkDescriptorPoolSize>               poolSize);

    VkDescriptorPoolSize DescriptorPoolSize(
            VkDescriptorType                    type,
            uint32_t                            count);



    SurfaceDisplayFormats GetPresentationInformation(
            VkPhysicalDevice                            physicalDevice,
            WindowObject                                window,
            VkSurfaceKHR                                surface);

    VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(
            VkPhysicalDevice        physicalDevice,
            VkSurfaceKHR            surface);

    VkPipelineViewportStateCreateInfo ViewportStateCreateInfo(
            std::vector<VkViewport>     viewports,
            std::vector<VkRect2D>       scissors);

    VkViewport Viewport(
            float       x,
            float       y,
            float       width,
            float       height,
            float       minDepth = 0.0f,
            float       maxDepth = 1.0f);

    VkViewport Viewport(float x,
                        float y,
                        VkExtent2D extent,
                        float minDepth = 0.0f,
                        float maxDepth = 1.0f);


    VkSwapchainCreateInfoKHR SwapchainCreateInfo(
            SurfaceDisplayFormats formats,
            VkSurfaceKHR window,
            uint32_t minImageCount,
            VkSurfaceTransformFlagBitsKHR transform, const std::vector<uint32_t> &queueIndex,
            uint32_t imageLayers = 1, VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

    VkComponentMapping ComponentMapping();

    VkImageSubresourceRange
    ImageSubresourceRange(VkImageAspectFlags aspect, uint32_t baseMipLevel = 0, uint32_t levelCount = 0,
                          uint32_t baseArrayLayer = 0, uint32_t layerCount = 0);

    VkImageViewCreateInfo
    ImageViewCreateInfo(VkImage image, VkImageViewType type, VkFormat format, VkComponentMapping components,
                        VkImageSubresourceRange range);

    VkShaderModuleCreateInfo ShaderModuleCreateInfo(std::vector<char> code);

    VkAttachmentDescription
    RenderPassAttachmentCreateInfo(VkFormat format, VkSampleCountFlagBits samples, VkImageLayout finalLayout);

    VkAttachmentReference RenderPassAttachmentReferenceCreateInfo(uint32_t attachment, VkImageLayout layout);

    VkSubpassDescription
    SubpassDescriptionCreateInfo(VkPipelineBindPoint bindPoint, VkAttachmentReference &colorAttachment);

    VkSubpassDependency
    SubpassDependencyCreateInfo(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask);

    VkRenderPassCreateInfo RenderPassCreateInfo(const std::vector<VkAttachmentDescription> &attachments,
                                                const std::vector<VkSubpassDescription> &subpasses,
                                                const std::vector<VkSubpassDependency> &dependancy);

    namespace Pipeline {
        VkPipelineShaderStageCreateInfo ShaderStageCreateInfo(Shader shader);

        /// TODO: Vertex Input Create Info (If Blank) Will Create A Rapidly Blinking Background For The Renderer
        VkPipelineVertexInputStateCreateInfo
        VertexInputCreateInfo(std::vector<VkVertexInputBindingDescription>& bindings,
                              std::vector<VkVertexInputAttributeDescription>& attributes);

        VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo(VkPrimitiveTopology topology);

        VkPipelineViewportStateCreateInfo Viewport(VkViewport viewport, VkRect2D scissor);

        /// TODO: When Created With This Info Function, A Flickery Red Screen Will Occur (Please Fix?)
        VkPipelineRasterizationStateCreateInfo
        RasterizationCreateInfo(VkBool32 useDepthClamp, VkPolygonMode drawMode, VkCullModeFlags cullmode,
                                VkFrontFace frontFace);

        VkPipelineMultisampleStateCreateInfo MultisampleCreateInfo(VkSampleCountFlagBits samples);

        VkPipelineColorBlendAttachmentState
        ColorBlendAttachmentCreateInfo(VkBool32 useBlend, VkBlendFactor srcColor, VkBlendFactor dstColor,
                                       VkBlendFactor srcAlpha, VkBlendFactor dstAlpha);

        VkPipelineColorBlendStateCreateInfo
        ColorBlendCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState> &attachments);

        VkPipelineLayoutCreateInfo LayoutCreateInfo(std::vector<VkDescriptorSetLayout> descriptorSetLayout,
                                                    std::vector<VkPushConstantRange> pushConstants);

        VkPipelineLayoutCreateInfo LayoutCreateInfo(VkDescriptorSetLayout descriptorSetLayout,
                                                    VkPushConstantRange pushConstants);

        VkFramebufferCreateInfo
        FramebufferCreateInfo(VkRenderPass renderpass, const std::vector<VkImageView> &attachments, VkExtent2D extent,
                              u32 layers);

        VkRenderPassBeginInfo RenderPassBeginInfo(const std::vector<VkClearValue> &clearValues, VkExtent2D extent,
                                                  const VkFramebuffer &buffer, VkRenderPass renderPass);

        VkSemaphoreCreateInfo SemaphoreCreateInfo();





        VkPresentInfoKHR PresentInfo(VkSwapchainKHR &swapchain, VkSemaphore &waitSemaphores, u32 &imageIndex);
    }

    VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);

    VkSubmitInfo SubmitInfo(
            VkSemaphore &waitSemaphores,
            VkSemaphore &signalSemaphores,
            VkCommandBuffer &Commandbuffers,
            std::vector<VkPipelineStageFlags> &waitFlags);

    VkFramebufferCreateInfo
    FramebufferCreateInfo(VkRenderPass renderpass, const std::vector<VkImageView> &attachments, VkExtent2D extent,
                          u32 layers);

    VkRenderPassBeginInfo
    RenderPassBeginInfo(const std::vector<VkClearValue> &clearValues, VkExtent2D extent, const VkFramebuffer &buffer,
                        VkRenderPass renderPass);

    VkSemaphoreCreateInfo SemaphoreCreateInfo();

    VkFenceCreateInfo FenceCreateInfo();

    VkSubmitInfo SubmitInfo(VkSemaphore &waitSemaphores, VkSemaphore &signalSemaphores, VkCommandBuffer &Commandbuffers,
                            std::vector<VkPipelineStageFlags> &waitFlags);

    VkPresentInfoKHR PresentInfo(VkSwapchainKHR& swapchain, VkSemaphore &waitSemaphores, u32 &imageIndex);

    VkBufferCreateInfo BufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode share = VK_SHARING_MODE_EXCLUSIVE);
    VkDescriptorSetAllocateInfo DescriptorAllocInfo(vector<VkDescriptorSetLayout>& layouts, VkDescriptorPool pool, u32 count);
    VkDescriptorSetAllocateInfo DescriptorAllocInfo(VkDescriptorSetLayout layouts, VkDescriptorPool pool, u32 count);


    VkSamplerCreateInfo SamplerCreateInfo(float maxAnisotropy = 0.0f);
    VkDescriptorSetLayoutBinding DescriptorLayoutBinding(u32 binding, u32 count, VkDescriptorType type, VkShaderStageFlags stages);
    VkVertexInputAttributeDescription VertexInputAttributeDescription(u32 binding, u32 loc,  u32 offset, VkFormat format);
    VkVertexInputBindingDescription VertexInputBindingDescription(u32 binding, u32 stride, VkVertexInputRate inputRate);
    VkDescriptorSetLayoutCreateInfo DescriptorLayoutCreateInfo(vector<VkDescriptorSetLayoutBinding> bindings);
    VkWriteDescriptorSet DescriptorWrite(VkDescriptorType type, u32 count, u32 binding, VkDescriptorSet* descriptor, VkDescriptorBufferInfo* bufferInfo = nullptr, VkDescriptorImageInfo* imageInfo = nullptr);
    VkDescriptorBufferInfo DescriptorBufferInfo(VkBuffer buffer, VkDeviceSize range, VkDeviceSize offset);
    VkPushConstantRange PushConstantRange(PipelineAccess access, u32 size, u32 offset);
    VkBufferCopy BufferCopyInfo(VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size);
}

#endif //ENGINE_INFOS_HPP
