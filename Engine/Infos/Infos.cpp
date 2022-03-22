//
// Created by Owner on 1/3/2022.
//

#include "Infos.hpp"

u32 findMemoryType(VkPhysicalDevice physicalDevice, u32 typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}
void CopyBuffer(VkDevice device, VkCommandPool pool, VkQueue queue, VkBuffer dst, VkBuffer src, VkDeviceSize size) {
    auto allocInfo = Info::CommandBufferAllocationInfo(pool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    VkCommandBuffer buffer;
    vkAllocateCommandBuffers(device, &allocInfo, &buffer);
    VkCommandBufferBeginInfo beginfo{};
    beginfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(buffer, &beginfo);
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(buffer, src, dst, 1, &copyRegion);
    vkEndCommandBuffer(buffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
    vkFreeCommandBuffers(device, pool, 1, &buffer);
}


Result Initalize() {
    if (!glfwInit()) return ERROR;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    return SUCCESS;
}


Shader CreateShader(VkDevice device, const std::string& path, VkShaderStageFlagBits type) {
    Shader shader;
    shader.Type = type;
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (file.is_open()) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        //auto info = Info::ShaderModuleCreateInfo(buffer);
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = buffer.size();
        info.pCode = reinterpret_cast<uint32_t*>(buffer.data());
        auto res = vkCreateShaderModule(device, &info, nullptr, &shader.Module);
        file.close();
    }
    return shader;
}

Result RecordCommandBuffer(VkCommandBuffer& Commandbuffer) {
    auto info = Info::CommandBufferBeginInfo();
    auto res = vkBeginCommandBuffer(Commandbuffer, &info);
    if (res != SUCCESS) return res;
    return SUCCESS;
}

Result EndCommandBuffer(VkCommandBuffer& Commandbuffer) { auto res = vkEndCommandBuffer(Commandbuffer); return res; }

Result BeginRenderPass(VkExtent2D extent, VkRenderPass renderPass, VkFramebuffer& Framebuffer, VkCommandBuffer& commandBuffer) {
    vector<VkClearValue> clearColor = {{0.02f,0.02f,0.02f,1.0f}};
    auto info = Info::RenderPassBeginInfo(clearColor, extent, Framebuffer, renderPass);
    vkCmdBeginRenderPass(commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    return SUCCESS;
}

Result EndRenderPass(VkCommandBuffer& Commandbuffer) { vkCmdEndRenderPass(Commandbuffer); return SUCCESS; }

namespace Info {
    namespace Debug {
        static u32 errorCount = 0;
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
            if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                errorCount++;
                printf("[Error: %i](%i) %s\n\n", errorCount, messageSeverity, pCallbackData->pMessage);
            }
            return VK_FALSE;
        }
        VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT callback) {
            VkDebugUtilsMessengerCreateInfoEXT info {
                    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                    .pNext = nullptr,
                    .flags = 0,
                    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                    .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                    .pfnUserCallback = callback,
                    .pUserData = nullptr
            }; return info;
        }
        Result CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }
    }
    VkApplicationInfo ApplicationCreateInfo(const char* appName, uint32_t appVersion, const char* engineName, uint32_t engineVersion) {
        VkApplicationInfo info{};
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        info.pNext = nullptr;
        info.pApplicationName = appName;
        info.applicationVersion = appVersion;
        info.pEngineName = engineName;
        info.engineVersion = engineVersion;
        info.apiVersion = VK_API_VERSION_1_2;
        return info;
    }
    VkInstanceCreateInfo InstanceCreateInfo(const VkApplicationInfo appInfo, const vector<const char*>& layers, const vector<const char*>& extensions) {
        VkInstanceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.pApplicationInfo = &appInfo;
        info.enabledLayerCount = static_cast<uint32_t>(layers.size());
        info.ppEnabledLayerNames = layers.data();
        info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        info.ppEnabledExtensionNames = extensions.data();
        return info;
    }
    VkDeviceQueueCreateInfo QueueCreateInfo(uint32_t familyIndex, uint32_t count, const float* priority) {
        VkDeviceQueueCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.queueFamilyIndex = familyIndex;
        info.queueCount = count;
        info.pQueuePriorities = priority;
        return info;
    }
    VkDeviceCreateInfo DeviceCreateInfo(const vector<VkDeviceQueueCreateInfo>& queues, const vector<const char*>& layers, const vector<const char*>& extensions, VkPhysicalDeviceFeatures* features) {
        VkDeviceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.queueCreateInfoCount = static_cast<uint32_t>(queues.size());
        info.pQueueCreateInfos = queues.data();
        info.enabledLayerCount = static_cast<uint32_t>(layers.size());
        info.ppEnabledLayerNames = layers.data();
        info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        info.ppEnabledExtensionNames = extensions.data();
        info.pEnabledFeatures = features;
        return info;
    }
    VkCommandPoolCreateInfo CommandPoolCreateInfo(u32 index, u32 flags) {
        VkCommandPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.queueFamilyIndex = index;
        return info;
    }
    VkCommandBufferAllocateInfo CommandBufferAllocationInfo(VkCommandPool pool, u32 bufferCount, VkCommandBufferLevel level) {
        VkCommandBufferAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext = nullptr;
        info.commandPool = pool;
        info.level = level;
        info.commandBufferCount = bufferCount;
        return info;
    }
    VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo(uint32_t maxSets, vector<VkDescriptorPoolSize> poolSize) {
        VkDescriptorPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.maxSets = maxSets;
        info.poolSizeCount = static_cast<uint32_t>(poolSize.size());
        info.pPoolSizes = poolSize.data();
        return info;
    }
    VkDescriptorPoolSize DescriptorPoolSize(VkDescriptorType type, uint32_t count) {
        VkDescriptorPoolSize info{};
        info.type = type;
        info.descriptorCount = count;
        return info;
    }
    VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags) {
        VkCommandBufferBeginInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.pInheritanceInfo = nullptr;
        return info;
    }
    SurfaceDisplayFormats GetPresentationInformation(VkPhysicalDevice physicalDevice, WindowObject window, VkSurfaceKHR surface) {
        SurfaceDisplayFormats Formats;

        uint32_t surfaceFormatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
        vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data());
        Formats.Format = surfaceFormats[0];
        for (auto i : surfaceFormats) {
            if (i.format == VK_FORMAT_B8G8R8A8_SRGB && i.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                Formats.Format = i;
                break;
            }
        }

        uint32_t presentCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentCount, nullptr);
        vector<VkPresentModeKHR> presentModes(presentCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentCount, presentModes.data());
        Formats.PresentMode = VK_PRESENT_MODE_FIFO_KHR;
        for (auto i : presentModes) {
            if (i == VK_PRESENT_MODE_MAILBOX_KHR) {
                Formats.PresentMode = i;
                break;
            }
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        Formats.Extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        return Formats;
    }
    VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
        VkSurfaceCapabilitiesKHR info;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &info);
        return info;
    }
    VkPipelineViewportStateCreateInfo ViewportStateCreateInfo(vector<VkViewport> viewports, vector<VkRect2D> scissors) {
        VkPipelineViewportStateCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.viewportCount = static_cast<uint32_t>(viewports.size());
        info.pViewports = viewports.data();
        info.scissorCount = static_cast<uint32_t>(scissors.size());
        info.pScissors = scissors.data();
        return info;
    }
    VkViewport Viewport(float x, float y, float width, float height, float minDepth, float maxDepth) {
        VkViewport info{};
        info.x = x;
        info.y = y;
        info.width = width;
        info.height = height;
        info.minDepth = minDepth;
        info.maxDepth = maxDepth;
        return info;
    }
    VkViewport Viewport(float x, float y, VkExtent2D extent, float minDepth, float maxDepth) {
        VkViewport info{};
        info.x = x;
        info.y = y;
        info.width = static_cast<float>(extent.width);
        info.height = static_cast<float>(extent.height);
        info.minDepth = minDepth;
        info.maxDepth = maxDepth;
        return info;
    }
    VkSwapchainCreateInfoKHR SwapchainCreateInfo(SurfaceDisplayFormats formats, VkSurfaceKHR Surface, uint32_t minImageCount,
                                                 VkSurfaceTransformFlagBitsKHR transform, const vector<uint32_t>& queueIndex,
                                                 uint32_t imageLayers, VkImageUsageFlags imageUsage) {
        VkSwapchainCreateInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.pNext = nullptr;
        info.flags = 0;
        info.surface = Surface;
        info.minImageCount = minImageCount;
        info.imageFormat = formats.Format.format;
        info.imageColorSpace = formats.Format.colorSpace;
        info.imageExtent = formats.Extent;
        info.imageArrayLayers = imageLayers;
        info.imageUsage = imageUsage;
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.queueFamilyIndexCount = static_cast<uint32_t>(queueIndex.size());
        info.pQueueFamilyIndices = queueIndex.data();
        info.preTransform = transform;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode = formats.PresentMode;
        info.clipped = VK_TRUE;
        info.oldSwapchain = nullptr;
        if (queueIndex.size() > 1) info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        return info;
    };
    VkComponentMapping ComponentMapping() {
        VkComponentMapping info{};
        info.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        return info;
    }
    VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspect, uint32_t baseMipLevel, uint32_t levelCount,
                                                  uint32_t baseArrayLayer, uint32_t layerCount) {
        VkImageSubresourceRange info{};
        info.aspectMask = aspect;
        info.baseMipLevel = baseMipLevel;
        info.levelCount = levelCount;
        info.baseArrayLayer = baseArrayLayer;
        info.layerCount = layerCount;
        return info;
    }
    VkImageViewCreateInfo ImageViewCreateInfo(VkImage image, VkImageViewType type, VkFormat format, VkComponentMapping components, VkImageSubresourceRange range) {
        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.image = image;
        info.viewType = type;
        info.format = format;
        info.components = components;
        info.subresourceRange = range;
        return info;
    }
    VkShaderModuleCreateInfo ShaderModuleCreateInfo(vector<char> code) {
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = code.size();
        info.pCode = reinterpret_cast<uint32_t*>(code.data());
        return info;
    }
    VkAttachmentDescription RenderPassAttachmentCreateInfo(VkFormat format, VkSampleCountFlagBits samples, VkImageLayout finalLayout) {
        VkAttachmentDescription info{};
        info.flags = 0;
        info.format = format;
        info.samples = samples;
        info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        info.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        info.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.finalLayout = finalLayout;
        return info;
    }
    VkAttachmentReference RenderPassAttachmentReferenceCreateInfo(uint32_t attachment, VkImageLayout layout) {
        VkAttachmentReference info{};
        info.attachment = attachment;
        info.layout = layout;
        return info;
    }
    VkSubpassDescription SubpassDescriptionCreateInfo(VkPipelineBindPoint bindPoint, VkAttachmentReference& colorAttachment) {
        VkSubpassDescription info{};
        info.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        info.colorAttachmentCount = 1;
        info.pColorAttachments = &colorAttachment;
        return info;
    }
    VkSubpassDependency SubpassDependencyCreateInfo( VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask,  VkAccessFlags dstAccessMask) {
        VkSubpassDependency info{};
        info.srcSubpass = VK_SUBPASS_EXTERNAL;
        info.dstSubpass = 0;
        info.srcStageMask = srcStageMask;
        info.dstStageMask = dstStageMask;
        info.srcAccessMask = srcAccessMask;
        info.dstAccessMask = dstAccessMask;
        info.dependencyFlags = 0;
        return info;
    }
    VkRenderPassCreateInfo RenderPassCreateInfo(const vector<VkAttachmentDescription>& attachments, const vector<VkSubpassDescription>& subpasses, const vector<VkSubpassDependency>& dependancy) {
        VkRenderPassCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.attachmentCount = static_cast<uint32_t>(attachments.size());
        info.pAttachments = attachments.data();
        info.subpassCount = static_cast<uint32_t>(subpasses.size());
        info.pSubpasses = subpasses.data();
        info.dependencyCount = static_cast<u32>(dependancy.size());
        info.pDependencies = dependancy.data();
        return info;
    }
    namespace Pipeline {
        VkPipelineShaderStageCreateInfo ShaderStageCreateInfo(Shader shader) {
            VkPipelineShaderStageCreateInfo info {};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.stage = shader.Type;
            info.module = shader.Module;
            info.pName = "main";
            info.pSpecializationInfo = nullptr;
            return info;
        }
        /// TODO: Vertex Input Create Info (If Blank) Will Create A Rapidly Blinking Background For The Renderer
        VkPipelineVertexInputStateCreateInfo VertexInputCreateInfo(vector<VkVertexInputBindingDescription>& bindings, vector<VkVertexInputAttributeDescription>& attributes) {
            VkPipelineVertexInputStateCreateInfo info {};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            if (bindings.empty()) info.vertexBindingDescriptionCount = 0; else {
                info.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
                info.pVertexBindingDescriptions = bindings.data();
            }
            if (attributes.empty()) info.vertexAttributeDescriptionCount = 0; else {
                info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
                info.pVertexAttributeDescriptions = attributes.data();
            }
            return info;
        }
        VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo(VkPrimitiveTopology topology) {
            VkPipelineInputAssemblyStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.topology = topology;
            info.primitiveRestartEnable = VK_FALSE;
            return info;
        }
        VkPipelineViewportStateCreateInfo Viewport(VkViewport viewport, VkRect2D scissor) {
            VkPipelineViewportStateCreateInfo info {};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.viewportCount = 1;
            info.pViewports = &viewport;
            info.scissorCount = 1;
            info.pScissors = &scissor;
            return info;
        }
        /// TODO: When Created With This Info Function, A Flickery Red Screen Will Occur (Please Fix?)
        VkPipelineRasterizationStateCreateInfo RasterizationCreateInfo(VkBool32 useDepthClamp, VkPolygonMode drawMode, VkCullModeFlags cullmode, VkFrontFace frontFace) {
            VkPipelineRasterizationStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.depthClampEnable = useDepthClamp;
            info.rasterizerDiscardEnable = VK_FALSE;
            info.polygonMode = drawMode;
            info.cullMode = cullmode;
            info.frontFace = frontFace;
            info.depthBiasEnable = VK_FALSE;
            info.depthBiasConstantFactor = 0.0f;
            info.depthBiasClamp = 0.0f;
            info.depthBiasSlopeFactor = 0.0f;
            info.lineWidth = 1.0f;
            return info;
        };
        VkPipelineMultisampleStateCreateInfo MultisampleCreateInfo(VkSampleCountFlagBits samples) {
            VkPipelineMultisampleStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.rasterizationSamples = samples;
            info.sampleShadingEnable = VK_FALSE;
            info.minSampleShading = 1.0f;
            info.pSampleMask = nullptr;
            info.alphaToCoverageEnable = VK_FALSE;
            info.alphaToOneEnable = VK_FALSE;
            return info;
        }
        VkPipelineColorBlendAttachmentState ColorBlendAttachmentCreateInfo(VkBool32 useBlend, VkBlendFactor srcColor, VkBlendFactor dstColor, VkBlendFactor srcAlpha, VkBlendFactor dstAlpha) {
            VkPipelineColorBlendAttachmentState info{};
            info.blendEnable = useBlend;
            info.srcColorBlendFactor = srcColor;
            info.dstColorBlendFactor = dstColor;
            info.colorBlendOp = VK_BLEND_OP_ADD;
            info.srcAlphaBlendFactor = srcAlpha;
            info.dstAlphaBlendFactor = dstAlpha;
            info.alphaBlendOp = VK_BLEND_OP_ADD;
            info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            return info;
        }
        VkPipelineColorBlendStateCreateInfo ColorBlendCreateInfo(const vector<VkPipelineColorBlendAttachmentState>& attachments) {
            VkPipelineColorBlendStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.logicOpEnable = VK_FALSE;
            info.logicOp = VK_LOGIC_OP_COPY;
            info.attachmentCount = static_cast<uint32_t>(attachments.size());
            info.pAttachments = attachments.data();
            return info;
        }
        VkPipelineLayoutCreateInfo LayoutCreateInfo(const vector<VkDescriptorSetLayout> descriptorSetLayouts, const vector<VkPushConstantRange> pushConstants) {
            VkPipelineLayoutCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            if (descriptorSetLayouts.empty()) {
                info.setLayoutCount = 0;
                info.pSetLayouts = VK_NULL_HANDLE;
            } else {
                info.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
                info.pSetLayouts = descriptorSetLayouts.data();
            }
            if (pushConstants.empty()) {
                info.pushConstantRangeCount = 0;
                info.pPushConstantRanges = VK_NULL_HANDLE;
            } else {
                info.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
                info.pPushConstantRanges = pushConstants.data();
            }
            return info;
        }
        VkPipelineLayoutCreateInfo LayoutCreateInfo(VkDescriptorSetLayout descriptorSetLayouts, VkPushConstantRange pushConstants) {
            VkPipelineLayoutCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.setLayoutCount = 1;
            info.pSetLayouts = &descriptorSetLayouts;
            info.pushConstantRangeCount = 1;
            info.pPushConstantRanges = &pushConstants;

            return info;
        }
    }
    VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass renderpass, const vector<VkImageView>& attachments,VkExtent2D extent, u32 layers) {
        VkFramebufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.renderPass = renderpass;
        info.attachmentCount = static_cast<u32>(attachments.size());
        info.pAttachments = attachments.data();
        info.width = extent.width;
        info.height = extent.height;
        info.layers = layers;
        return info;
    }
    VkRenderPassBeginInfo RenderPassBeginInfo(const vector<VkClearValue>& clearValues, VkExtent2D extent, const VkFramebuffer& buffer, VkRenderPass renderPass) {
        VkRenderPassBeginInfo info{};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.pNext = nullptr;
        info.renderPass = renderPass;
        info.framebuffer = buffer;
        info.renderArea = {.offset = {0,0}, .extent = extent};
        info.clearValueCount = static_cast<u32>(clearValues.size());
        info.pClearValues = clearValues.data();
        return info;
    }
    VkSemaphoreCreateInfo SemaphoreCreateInfo() {
        VkSemaphoreCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        return info;
    }
    VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags) {
        VkFenceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        return info;
    }
    VkSubmitInfo SubmitInfo(VkSemaphore& waitSemaphores, VkSemaphore& signalSemaphores, VkCommandBuffer& Commandbuffers, vector<VkPipelineStageFlags>& waitFlags) {
        VkSubmitInfo info {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pNext = nullptr;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &waitSemaphores;
        info.pWaitDstStageMask = waitFlags.data();
        info.commandBufferCount = 1;
        info.pCommandBuffers = &Commandbuffers;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &signalSemaphores;
        return info;
    }
    VkPresentInfoKHR PresentInfo(VkSwapchainKHR& swapchain, VkSemaphore& waitSemaphores, u32& imageIndex) {
        VkPresentInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.pNext = nullptr;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &waitSemaphores;
        info.swapchainCount = 1;
        info.pSwapchains = &swapchain;
        info.pImageIndices = &imageIndex;
        info.pResults = nullptr;
        return info;
    }
    VkBufferCreateInfo BufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode share) {
        VkBufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.size = size;
        info.usage = usage;
        info.sharingMode = share;
        return info;
    }
    VkDescriptorSetAllocateInfo DescriptorAllocInfo(vector<VkDescriptorSetLayout>& layouts, VkDescriptorPool pool, u32 count) {
        VkDescriptorSetAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.descriptorPool = pool;
        info.descriptorSetCount = count;
        info.pSetLayouts = layouts.data();
        return info;
    }
    VkDescriptorSetAllocateInfo DescriptorAllocInfo(VkDescriptorSetLayout layouts, VkDescriptorPool pool, u32 count) {
        VkDescriptorSetAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.descriptorPool = pool;
        info.descriptorSetCount = 1;
        info.pSetLayouts = &layouts;
        return info;
    }
    VkSamplerCreateInfo SamplerCreateInfo(float maxAnisotropy) {
        VkSamplerCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.pNext = VK_NULL_HANDLE;
        info.flags = 0;
        info.magFilter = FILTER_LINEAR;
        info.minFilter = FILTER_LINEAR;
        info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        info.addressModeU = SAMPLER_REPEAT;
        info.addressModeV = SAMPLER_REPEAT;
        info.addressModeW = SAMPLER_REPEAT;
        info.mipLodBias = 0.0f;
        if (maxAnisotropy > 0.0f) {
            info.anisotropyEnable = VK_TRUE;
            info.maxAnisotropy = maxAnisotropy;
        } else {
            info.anisotropyEnable = VK_FALSE;
            info.maxAnisotropy = 1.0f;
        }
        info.compareEnable = VK_FALSE;
        info.compareOp = VK_COMPARE_OP_ALWAYS;
        info.minLod = 0.0f;
        info.maxLod = 0.0f;
        info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        info.unnormalizedCoordinates = VK_FALSE;
        return info;
    }
    VkDescriptorSetLayoutBinding DescriptorLayoutBinding(u32 binding, u32 count, VkDescriptorType type, VkShaderStageFlags stages) {
        VkDescriptorSetLayoutBinding info{};
        info.stageFlags = stages;
        info.pImmutableSamplers = nullptr;
        info.descriptorType = type;
        info.descriptorCount = count;
        info.binding = binding;
        return info;
    }
    VkVertexInputAttributeDescription VertexInputAttributeDescription(u32 binding, u32 loc,  u32 offset, VkFormat format) {
        VkVertexInputAttributeDescription info{};
        info.binding = binding;
        info.offset = offset;
        info.format = format;
        info.location = loc;
        return info;
    }
    VkVertexInputBindingDescription VertexInputBindingDescription(u32 binding, u32 stride, VkVertexInputRate inputRate) {
        VkVertexInputBindingDescription info{};
        info.binding = binding;
        info.inputRate = inputRate;
        info.stride = stride;
        return info;
    }
    VkDescriptorSetLayoutCreateInfo DescriptorLayoutCreateInfo(vector<VkDescriptorSetLayoutBinding> bindings) {
        VkDescriptorSetLayoutCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.flags = 0;
        info.pNext = nullptr;
        info.bindingCount = static_cast<u32>(bindings.size());
        info.pBindings = bindings.data();
        return info;
    }
    VkWriteDescriptorSet DescriptorWrite(VkDescriptorType type, u32 count, u32 binding, VkDescriptorSet* descriptor, VkDescriptorBufferInfo* bufferInfo, VkDescriptorImageInfo* imageInfo) {
        VkWriteDescriptorSet info{};
        info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        info.pNext = nullptr;
        info.descriptorType = type;
        info.descriptorCount = count;
        info.dstArrayElement = 0;
        info.dstBinding = binding;
        info.dstSet = *descriptor;
        info.pBufferInfo = bufferInfo;
        info.pImageInfo = imageInfo;
        return info;
    }
    VkDescriptorBufferInfo DescriptorBufferInfo(VkBuffer buffer, VkDeviceSize range, VkDeviceSize offset) {
        VkDescriptorBufferInfo info{};
        info.offset = offset;
        info.buffer = buffer;
        info.range = range;
        return info;
    }
    VkPushConstantRange PushConstantRange(PipelineAccess access, u32 size, u32 offset) {
        VkPushConstantRange info{};
        info.stageFlags = access;
        info.offset = offset;
        info.size = size;
        return info;
    }
    VkBufferCopy BufferCopyInfo(VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size) {
        VkBufferCopy info{};
        info.dstOffset = dstOffset;
        info.srcOffset = srcOffset;
        info.size = size;
        return info;
    }
}

Result CreateInstance(VkInstance& instance, bool debug) {
    Initalize();
    auto appInfo = Info::ApplicationCreateInfo("App", 1);

    uint32_t extensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extensionCount);
    std::vector<const char*> layers{};

    VkDebugUtilsMessengerEXT debugMessenger{};
    auto debugInfo = Info::Debug::DebugMessengerCreateInfo(Info::Debug::DebugCallback);
    if (debug) { extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); layers.push_back("VK_LAYER_KHRONOS_validation"); }

    auto instanceInfo = Info::InstanceCreateInfo(appInfo, layers, extensions);
    if (debug) instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugInfo;

    auto res = vkCreateInstance(&instanceInfo, nullptr, &instance);
    if (res != SUCCESS) return res;
    if (debug) {
        auto debugRes = Info::Debug::CreateDebugUtilsMessengerEXT(instance, &debugInfo, nullptr, &debugMessenger);
        if (debugRes != SUCCESS) return debugRes;
    }
    return res;
}
VkInstance CreateInstance(bool debug) {
    VkInstance instance{};
    auto res = CreateInstance(instance, debug);
    return instance;
}

// Unused Texture Code, Need To Reimplement
/*
 void                        createImage(ws::Device device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, ws::Texture& image) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device.get(), &imageInfo, nullptr, &image.Image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device.get(), image.Image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(device.getPhysicalDevice(), memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device.get(), &allocInfo, nullptr, &image.Memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device.get(), image.Image, image.Memory, 0);
}
void                        copyBufferToImage(ws::Device device, VkCommandPool pool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, pool);
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
            width,
            height,
            1
    };
    vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
    );
    endSingleTimeCommands(device, device.getGraphicsQueue(), pool, commandBuffer);
}
void                        transitionImageLayout(ws::Device device, ws::Texture image, VkCommandPool pool, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer buffer = beginSingleTimeCommands(device, pool);
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image.Image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
            buffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
    );

    endSingleTimeCommands(device, device.getGraphicsQueue(), pool, buffer);
}
ws::Texture                 ws::createTexture(ws::Device device, ws::CommandPool pool, const char* path) {
    ws::Texture texture;
    int width, height, channels;
    stbi_uc* pixels = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    VkDeviceSize imageSize = width * height * STBI_rgb_alpha;
    if (!pixels) {
        throw std::runtime_error("Failed To Load Image");
    }
    auto stagingBuffer = CREATE_STAGING_BUFFER(device.get(), device.getPhysicalDevice(), imageSize);
    UPLOAD_BUFFER_TO_MEMORY(device.get(), stagingBuffer.Memory, pixels, imageSize);
    stbi_image_free(pixels);

    createImage(device, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture);

    transitionImageLayout(device, texture, pool.get(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(device, pool.get(), stagingBuffer.Buffer, texture.Image, width, height);
    transitionImageLayout(device, texture, pool.get(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device.get(), stagingBuffer.Buffer, nullptr);
    vkFreeMemory(device.get(), stagingBuffer.Memory, nullptr);

    auto subResourceRange = Info::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
    auto viewInfo = Info::ImageViewCreateInfo(texture.Image, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, Info::ComponentMapping(), subResourceRange);
    auto viewRes = vkCreateImageView(device.get(), &viewInfo, nullptr, &texture.View);


    return texture;
}
 */




