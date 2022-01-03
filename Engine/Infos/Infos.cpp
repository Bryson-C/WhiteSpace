//
// Created by Owner on 1/3/2022.
//

#include "Infos.hpp"


namespace Info {
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
    VkInstanceCreateInfo InstanceCreateInfo(const VkApplicationInfo appInfo, const std::vector<const char*>& layers, const std::vector<const char*>& extensions) {
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
    VkDeviceCreateInfo DeviceCreateInfo(const std::vector<VkDeviceQueueCreateInfo>& queues, const std::vector<const char*>& layers, const std::vector<const char*>& extensions, VkPhysicalDeviceFeatures* features) {
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
    VkCommandPoolCreateInfo CommandPoolCreateInfo(u32 index) {
        VkCommandPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
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
    VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo(uint32_t maxSets, std::vector<VkDescriptorPoolSize> poolSize) {
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
    VkCommandBufferBeginInfo CommandBufferBeginInfo() {
        VkCommandBufferBeginInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.pInheritanceInfo = nullptr;
        return info;
    }
    SurfaceDisplayFormats GetPresentationInformation(VkPhysicalDevice physicalDevice, WindowObject window, VkSurfaceKHR surface) {
        SurfaceDisplayFormats Formats;

        uint32_t surfaceFormatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
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
        std::vector<VkPresentModeKHR> presentModes(presentCount);
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
    VkPipelineViewportStateCreateInfo ViewportStateCreateInfo(std::vector<VkViewport> viewports, std::vector<VkRect2D> scissors) {
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
                                                 VkSurfaceTransformFlagBitsKHR transform, const std::vector<uint32_t>& queueIndex,
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
    VkShaderModuleCreateInfo ShaderModuleCreateInfo(std::vector<char> code) {
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
    VkRenderPassCreateInfo RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachments, const std::vector<VkSubpassDescription>& subpasses, const std::vector<VkSubpassDependency>& dependancy) {
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
        VkPipelineVertexInputStateCreateInfo VertexInputCreateInfo(std::vector<VkVertexInputBindingDescription> bindings, std::vector<VkVertexInputAttributeDescription> attributes) {
            VkPipelineVertexInputStateCreateInfo info {};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            if (!bindings.empty()) {
                info.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
                info.pVertexBindingDescriptions = bindings.data();
            } else {
                info.vertexBindingDescriptionCount = 0;
            }
            if (!attributes.empty()) {
                info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
                info.pVertexAttributeDescriptions = attributes.data();
            } else {
                info.vertexAttributeDescriptionCount = 0;
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
        VkPipelineColorBlendStateCreateInfo ColorBlendCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments) {
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
        VkPipelineLayoutCreateInfo LayoutCreateInfo(const std::vector<VkDescriptorSetLayout>& descriptorSets, const std::vector<VkPushConstantRange>& pushConstants) {
            VkPipelineLayoutCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.setLayoutCount = static_cast<uint32_t>(descriptorSets.size());
            info.pSetLayouts = descriptorSets.data();
            info.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
            info.pPushConstantRanges = pushConstants.data();
            return info;
        }
    }
    VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass renderpass, const std::vector<VkImageView>& attachments,VkExtent2D extent, u32 layers) {
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
    VkRenderPassBeginInfo RenderPassBeginInfo(const std::vector<VkClearValue>& clearValues, VkExtent2D extent, const VkFramebuffer& buffer, VkRenderPass renderPass) {
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
    VkFenceCreateInfo FenceCreateInfo() {
        VkFenceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        return info;
    }
    VkSubmitInfo SubmitInfo(VkSemaphore& waitSemaphores, VkSemaphore& signalSemaphores, VkCommandBuffer& Commandbuffers, std::vector<VkPipelineStageFlags>& waitFlags) {
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

}
