#include "Engine.hpp"

namespace Info {
    namespace Debug {
        static uint32_t errorCount = 0;
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
            if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT && settingDefined(settings::Debug)) {
                errorCount++;
                if (settingDefined(settings::LogErrorCount)) printf("Error Count: %i\n", errorCount);
                printf("(%i) %s\n\n", messageSeverity, pCallbackData->pMessage);
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

}

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

Result Initalize() {
    if (!glfwInit()) return ERROR;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    return SUCCESS;
}
Result CreateInstance(VkInstance& instance) {
    Initalize();
    auto appInfo = Info::ApplicationCreateInfo("App", 1);

    uint32_t extensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extensionCount);
    std::vector<const char*> layers{};

    VkDebugUtilsMessengerEXT debugMessenger{};
    auto debugInfo = Info::Debug::DebugMessengerCreateInfo(Info::Debug::DebugCallback);
    if (settingDefined(settings::Debug)) { extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); layers.push_back("VK_LAYER_KHRONOS_validation"); }

    auto instanceInfo = Info::InstanceCreateInfo(appInfo, layers, extensions);
    if (settingDefined(settings::Debug)) instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugInfo;

    auto res = vkCreateInstance(&instanceInfo, nullptr, &instance);
    if (res != SUCCESS) return res;
    if (settingDefined(settings::Debug)) {
        auto debugRes = Info::Debug::CreateDebugUtilsMessengerEXT(instance, &debugInfo, nullptr, &debugMessenger);
        if (debugRes != SUCCESS) return debugRes;
    }
    return res;
}
VkInstance CreateInstance() {
    VkInstance instance{};
    auto res = CreateInstance(instance);
    return instance;
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


BufferObject CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlag) {
    BufferObject Buffer;
    auto createInfo = Info::BufferCreateInfo(size, usage);
    vkCreateBuffer(device, &createInfo, nullptr, &Buffer.Buffer);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, Buffer.Buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, memoryFlag);
    auto res = vkAllocateMemory(device, &allocInfo, nullptr, &Buffer.Memory);
    vkBindBufferMemory(device, Buffer.Buffer, Buffer.Memory, 0);
    return Buffer;
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

ws::Instance::Instance(const char* name, bool debug) : m_InstanceName(name), m_Debug(debug) {
    Initalize();
    if (debug) addSetting(settings::Debug);
    m_Instance = CreateInstance();
}
VkInstance ws::Instance::get() { return m_Instance; }
const char* ws::Instance::getName() { return m_InstanceName; }


static void                 resizeCallback(GLFWwindow*, int, int) {}
                            ws::Window::Window(ws::Instance& instance, uint32_t width, uint32_t height) {
    m_Window = glfwCreateWindow(width, height, instance.getName(), nullptr, nullptr);
    glfwCreateWindowSurface(instance.get(), m_Window, nullptr, &m_Surface);
    glfwSetFramebufferSizeCallback(m_Window, resizeCallback);
}
bool                        ws::Window::isOpen() { return !glfwWindowShouldClose(m_Window); }
void                        ws::Window::swapBuffers() { glfwSwapBuffers(m_Window); }
void                        ws::Window::pollEvents() { glfwPollEvents(); }
GLFWwindow*                 ws::Window::get() { return m_Window; }
VkSurfaceKHR                ws::Window::getSurface() { return m_Surface; }
v2<uint32_t>                ws::Window::size() const {
    int w,h;
    glfwGetFramebufferSize(m_Window, &w, &h);
    return {static_cast<u32>(w),static_cast<u32>(h)};
}

VkPhysicalDevice            FindPhysicalDevice(VkInstance instance) {
    u32 count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(instance, &count, devices.data());
    for (auto i : devices) {
        return i;
    }
}
vector<VkQueueFamilyProperties> FindQueueFamilyProperties(VkPhysicalDevice physicalDevice) {
    u32 count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
    vector<VkQueueFamilyProperties> queueFamilies(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilies.data());
    return queueFamilies;
}
bool                        deviceSupportsSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, u32 index) {
    VkBool32 support;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, surface, &support);
    return support;
}

                            ws::Device::Device(ws::Instance& instance, Window& window) {
    m_PhysicalDevice = FindPhysicalDevice(instance.get());
    auto queueFamilyProps = FindQueueFamilyProperties(m_PhysicalDevice);

    u32 loop = 0;
    for (auto& i : queueFamilyProps) {
        if (i.queueFlags & qGRAPHICS_FLAG) { m_GraphicsIndex = loop; }
        if (deviceSupportsSurface(m_PhysicalDevice, window.getSurface(), loop)) { m_PresentIndex = loop; }
        loop++;
    }
    const float priority = 1.0f;
    vector<VkDeviceQueueCreateInfo> qInfos{};
    qInfos.emplace_back(Info::QueueCreateInfo(m_GraphicsIndex, 1, &priority));
    if (m_PresentIndex != m_GraphicsIndex) qInfos.emplace_back(Info::QueueCreateInfo(m_PresentIndex, 1, &priority));
    vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    auto deviceInfo = Info::DeviceCreateInfo(qInfos, {}, extensions);
    vkCreateDevice(m_PhysicalDevice, &deviceInfo, nullptr, &m_Device);

    vkGetDeviceQueue(m_Device, m_GraphicsIndex, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, m_PresentIndex, 0, &m_PresentQueue);
}
VkDevice                    ws::Device::get() { return m_Device; }
VkPhysicalDevice            ws::Device::getPhysicalDevice() { return m_PhysicalDevice; }
u32                         ws::Device::getGraphicsIndex() { return m_GraphicsIndex; }
VkQueue                     ws::Device::getGraphicsQueue() { return m_GraphicsQueue; }
VkResult                    ws::Device::submitQueue(VkSubmitInfo& submitInfo, VkFence fence) {
    return vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, fence);
}
VkResult                    ws::Device::presentQueue(VkPresentInfoKHR& presentInfo) {
    return vkQueuePresentKHR(m_GraphicsQueue, &presentInfo);
}
VkResult                    ws::Device::waitForFences(VkFence* fences, u32 count) {
    return vkWaitForFences(m_Device, count, fences, VK_TRUE, UINT64_MAX);
}
VkResult                    ws::Device::resetFences(VkFence* fences, u32 count) {
    return vkResetFences(m_Device, count, fences);
}
vector<VkSemaphore>           ws::Device::createSemaphores(uint32_t count) {
    vector<VkSemaphore> semaphores(count, nullptr);
    for (auto& i : semaphores) {
        auto info = Info::SemaphoreCreateInfo();
        auto res = vkCreateSemaphore(m_Device, &info, nullptr, &i);
    }
    return semaphores;
}
vector<VkFence>               ws::Device::createFences(uint32_t count) {
    vector<VkFence> fences(count);
    for (auto& i : fences) {
        auto info = Info::FenceCreateInfo();
        auto res = vkCreateFence(m_Device, &info, nullptr, &i);
    }
    return fences;
}
void                        ws::Device::waitIdle() {
    vkDeviceWaitIdle(m_Device);
}


VkSurfaceFormatKHR          getSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    u32 count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
    vector<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, formats.data());
    for (auto& i : formats) {
        if (i.format == VK_FORMAT_B8G8R8A8_SRGB && i.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return i;
        }
    }
    return formats[0];
}
VkPresentModeKHR            getPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    u32 count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
    vector<VkPresentModeKHR> presentModes(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, presentModes.data());
    for (auto& i : presentModes) {
        if (i == VK_PRESENT_MODE_MAILBOX_KHR) return i;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}
u32                         getMinimumImageCount(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    u32 imageCount = 0;
    VkSurfaceCapabilitiesKHR capa;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capa);
    imageCount = capa.minImageCount + 1;
    if (capa.maxImageCount > 0 && imageCount > capa.maxImageCount) {
        imageCount = capa.maxImageCount;
    }
    return imageCount;
}
ws::ImageArray               CreateSwapchainImagesAndViews(VkDevice device, VkSwapchainKHR swapchain, VkFormat format) {
    ws::ImageArray Images;
    u32 imageCount = 0;
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    Images.Image.resize(imageCount);
    Images.View.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, Images.Image.data());

    for (u32 i = 0; i < Images.Image.size(); i++) {
        auto range = Info::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
        auto component = Info::ComponentMapping();
        auto info = Info::ImageViewCreateInfo(Images.Image[i], VK_IMAGE_VIEW_TYPE_2D, format, component, range);
        vkCreateImageView(device, &info, nullptr, &Images.View[i]);
    }
    return Images;
}



struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
SwapChainSupportDetails     querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}
VkSurfaceFormatKHR          chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}
VkPresentModeKHR            chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D                  chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
void                        ws::Swapchain::create(ws::Device& device, ws::Window& window) {
    m_Formats.Extent = {window.size().x, window.size().y};
    m_Formats.Format = getSurfaceFormat(device.getPhysicalDevice(), window.getSurface());
    m_Formats.PresentMode = getPresentMode(device.getPhysicalDevice(), window.getSurface());
    auto capa = Info::GetSurfaceCapabilities(device.getPhysicalDevice(), window.getSurface());
    u32 minImageCount = getMinimumImageCount(device.getPhysicalDevice(), window.getSurface());
    vector<u32> queueIndices = { device.getGraphicsIndex() };

    auto createInfo = Info::SwapchainCreateInfo(m_Formats, window.getSurface(), minImageCount, capa.currentTransform, queueIndices);

    vkCreateSwapchainKHR(device.get(), &createInfo, nullptr, &m_Swapchain);
    m_Images = CreateSwapchainImagesAndViews(device.get(), m_Swapchain, m_Formats.Format.format);
}
                            ws::Swapchain::Swapchain(ws::Device& device, ws::Window& window) {
    create(device, window);
}

VkSwapchainKHR              ws::Swapchain::get() { return m_Swapchain; }
VkExtent2D                  ws::Swapchain::getExtent() { return m_Formats.Extent; }
SurfaceDisplayFormats       ws::Swapchain::getFormats() { return m_Formats; }
u32                         ws::Swapchain::getImageCount() { return m_Images.Image.size(); }
VkFramebuffer*              ws::Swapchain::getFramebuffers() { return m_Framebuffers.data(); };
void                        ws::Swapchain::initializeFramebuffers(ws::Device& device, VkRenderPass& renderPass) {
    m_Framebuffers.resize(m_Images.Image.size());
    for (int i = 0; i < m_Framebuffers.size(); i++) {
        std::vector<VkImageView> attachments = {m_Images.View[i]};
        auto info = Info::FramebufferCreateInfo(renderPass, attachments, m_Formats.Extent, 1);
        vkCreateFramebuffer(device.get(), &info, nullptr, &m_Framebuffers[i]);
    }
}
void                        ws::Swapchain::destroyFramebuffers(ws::Device device) {
    for (auto& i : m_Framebuffers) {
        vkDestroyFramebuffer(device.get(), i, nullptr);
    }
}
void                        ws::Swapchain::destroyImageViews(ws::Device device) {
    for (auto& i : m_Images.View) {
        vkDestroyImageView(device.get(), i, nullptr);
    }
}
void                        ws::Swapchain::destroySwapchain(ws::Device device) {
    vkDestroySwapchainKHR(device.get(), m_Swapchain, nullptr);
}
u32                         ws::Swapchain::getImageIndex(ws::Device device, VkSemaphore& semaphore, Result* result) {
    u32 index = 0;
    *result = vkAcquireNextImageKHR(device.get(), m_Swapchain, UINT64_MAX, semaphore, nullptr, &index);
    return index;
}

                            ws::CommandBuffer::CommandBuffer(ws::Device &device, VkCommandPool* owner, uint32_t count) : m_OwnerPool(owner), m_Index(0) {
    m_Buffers.resize(count);
    auto info = Info::CommandBufferAllocationInfo(*m_OwnerPool, count, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    auto res = vkAllocateCommandBuffers(device.get(), &info, m_Buffers.data());
}
void                        ws::CommandBuffer::nextIndex() { if (m_Index < m_Buffers.size()-1) m_Index++; }
void                        ws::CommandBuffer::prevIndex() { if (m_Index != 0) m_Index--; }
void                        ws::CommandBuffer::setIndex(u32 index) { m_Index = index; }
void                        ws::CommandBuffer::record(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent) {
    RecordCommandBuffer(m_Buffers[m_Index]);
    BeginRenderPass(extent, renderPass, framebuffer, m_Buffers[m_Index]);
}
void                        ws::CommandBuffer::stopRecording() {
    EndRenderPass(m_Buffers[m_Index]);
    EndCommandBuffer(m_Buffers[m_Index]);
}
void                        ws::CommandBuffer::bindPipeline(VkPipeline pipeline, VkPipelineBindPoint bindPoint) {
    vkCmdBindPipeline(m_Buffers[m_Index], bindPoint, pipeline);
}
void                        ws::CommandBuffer::draw(u32 vertexCount, u32 instances, u32 firstVertex, u32 firstInstance) {
    vkCmdDraw(m_Buffers[m_Index], vertexCount, instances, firstVertex, firstInstance);
}
void                        ws::CommandBuffer::drawIndexed(u32 indexCount, u32 instances, u32 firstIndex , u32 firstVertex, u32 firstInstance) {
    vkCmdDrawIndexed(m_Buffers[m_Index], indexCount, instances, firstIndex, firstVertex, firstInstance);
}
void                        ws::CommandBuffer::bindVertexBuffer(VkBuffer buffer) {
    VkDeviceSize offset = {0};
    vkCmdBindVertexBuffers(m_Buffers[m_Index], 0, 1, &buffer, &offset);
}
void                        ws::CommandBuffer::bindIndexBuffer(VkBuffer buffer, VkIndexType type) {
    VkDeviceSize offset = {0};
    vkCmdBindIndexBuffer(m_Buffers[m_Index], buffer, offset, type);
}
void                        ws::CommandBuffer::bindDesciptorSets(VkPipelineLayout layout, VkDescriptorSet set) {
    vkCmdBindDescriptorSets(m_Buffers[m_Index], VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &set, 0, nullptr);
}
VkCommandBuffer             ws::CommandBuffer::get(uint32_t index) {
    return m_Buffers[index];
}
void                        ws::CommandBuffer::freeBuffers(ws::Device device) {
    vkFreeCommandBuffers(device.get(), *m_OwnerPool, m_Buffers.size(), m_Buffers.data());
}
void                        ws::CommandPool::destroy(ws::Device device) {
    vkDestroyCommandPool(device.get(), m_Pool, nullptr);
}



                            ws::CommandPool::CommandPool(ws::Device device) {
    auto createInfo = Info::CommandPoolCreateInfo(device.getGraphicsIndex());
    vkCreateCommandPool(device.get(), &createInfo, nullptr, &m_Pool);
}
                            ws::CommandBuffer ws::CommandPool::AllocateBuffers(ws::Device& device, u32 count) {
    auto buffers = CommandBuffer(device, &m_Pool, count);
    m_Buffers = &buffers;
    return buffers;
}
VkCommandPool               ws::CommandPool::get() {
    return m_Pool;
}

                            ws::RenderPass::RenderPass(ws::Device& device, ws::Swapchain& swapchain) {
    std::vector<VkAttachmentDescription> colorAttchment = {Info::RenderPassAttachmentCreateInfo(swapchain.getFormats().Format.format, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)};
    auto colorAttachmentRef = Info::RenderPassAttachmentReferenceCreateInfo(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    std::vector<VkSubpassDescription> subpass = {Info::SubpassDescriptionCreateInfo(VK_PIPELINE_BIND_POINT_GRAPHICS, colorAttachmentRef)};
    std::vector<VkSubpassDependency> dependancy = {Info::SubpassDependencyCreateInfo(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)};
    auto info = Info::RenderPassCreateInfo(colorAttchment, subpass, dependancy);

    VkResult renderpassResult = vkCreateRenderPass(device.get(), &info, nullptr, &m_RenderPass);
}
VkRenderPass                ws::RenderPass::get() const { return m_RenderPass; }



ws::UniformBuffer::UniformBuffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, u32 count) {

    m_Buffer.resize(count);
    m_Memory.resize(count);
    m_Size = size;
    for (int i = 0; i < m_Buffer.size(); i++) {
        auto buffer = CreateBuffer(device.get(), device.getPhysicalDevice(), size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, RAM_MEMORY | COHERENT_MEMORY);
        m_Buffer[i] = buffer.Buffer;
        m_Memory[i] = buffer.Memory;
    }

}
vector<VkDeviceMemory>        ws::UniformBuffer::getMem() {
    return m_Memory;
}
void                        ws::UniformBuffer::update(ws::Device device, void* data, u32 index) {

    if (index > m_Buffer.size()) {
        throw std::runtime_error("Index Was Larger Than Buffer Size");
    }

    void* upload;
    vkMapMemory(device.get(), m_Memory[index], 0, m_Size, 0, &upload);
    memcpy(upload, &data, m_Size);
    vkUnmapMemory(device.get(), m_Memory[index]);
}
VkBuffer*                   ws::UniformBuffer::get() {
    return m_Buffer.data();
}
void                        ws::UniformBuffer::destroy(ws::Device device) {
    for (int i = 0; i < m_Buffer.size(); i++) {
        vkFreeMemory(device.get(), m_Memory[i], nullptr);
        vkDestroyBuffer(device.get(), m_Buffer[i], nullptr);
    }
}
VkDeviceSize                ws::UniformBuffer::getSize() { return m_Size; }


                            ws::GraphicsPipeline::GraphicsPipeline(ws::Device device, ws::Swapchain swapchain, ws::RenderPass renderPass, Shader vShader, Shader fShader) {
    auto vertexInfo = Info::Pipeline::ShaderStageCreateInfo(vShader);
    auto fragmentInfo = Info::Pipeline::ShaderStageCreateInfo(fShader);
    VkPipelineShaderStageCreateInfo shaderStages[2] = {vertexInfo,fragmentInfo};

    vector<VkVertexInputAttributeDescription> attribs{};
    vector<VkVertexInputBindingDescription > bindings{};
    auto vertexInput = Info::Pipeline::VertexInputCreateInfo(bindings,attribs);
    auto inputAssembly = Info::Pipeline::InputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    auto viewportInfo = Info::Viewport(0.0f,0.0f,swapchain.getExtent(), 0.0f, 1.0f);
    VkRect2D scissorInfo = {{0,0}, swapchain.getExtent()};
    auto viewport = Info::Pipeline::Viewport(viewportInfo, scissorInfo);
    auto rasterization = Info::Pipeline::RasterizationCreateInfo(VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
    auto multisample = Info::Pipeline::MultisampleCreateInfo(VK_SAMPLE_COUNT_1_BIT);
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachment = {Info::Pipeline::ColorBlendAttachmentCreateInfo(VK_FALSE, BLEND_ONE, BLEND_ONE, BLEND_ZERO, BLEND_ZERO)};
    auto colorBlend = Info::Pipeline::ColorBlendCreateInfo(colorBlendAttachment);



    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    auto pipelineLayout = Info::Pipeline::LayoutCreateInfo({}, {});
    auto res = vkCreatePipelineLayout(device.get(), &pipelineLayout, nullptr, &m_Layout);

    VkGraphicsPipelineCreateInfo pipelineInfo {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInput,
            .pInputAssemblyState = &inputAssembly,
            .pTessellationState = nullptr,
            .pViewportState = &viewport,
            .pRasterizationState = &rasterization,
            .pMultisampleState = &multisample,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlend,
            .pDynamicState = nullptr,
            .layout = m_Layout,
            .renderPass = renderPass.get(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
    };
    auto pipelineRes = vkCreateGraphicsPipelines(device.get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);
}
                            ws::GraphicsPipeline::GraphicsPipeline(ws::Device device, ws::Swapchain swapchain, ws::RenderPass renderPass, Shader vShader, Shader fShader,
                                                                    vector<VkVertexInputAttributeDescription>& attributes,
                                                                    vector<VkVertexInputBindingDescription>& bindings,
                                                                    ws::UniformBuffer& uniformBuffers) {
    auto vertexInfo = Info::Pipeline::ShaderStageCreateInfo(vShader);
    auto fragmentInfo = Info::Pipeline::ShaderStageCreateInfo(fShader);
    VkPipelineShaderStageCreateInfo shaderStages[2] = {vertexInfo,fragmentInfo};

    auto vertexInput = Info::Pipeline::VertexInputCreateInfo(bindings, attributes);

    auto inputAssembly = Info::Pipeline::InputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    auto viewportInfo = Info::Viewport(0.0f,0.0f,swapchain.getExtent(), 0.0f, 1.0f);
    VkRect2D scissorInfo = {{0,0}, swapchain.getExtent()};
    auto viewport = Info::Pipeline::Viewport(viewportInfo, scissorInfo);
    auto rasterization = Info::Pipeline::RasterizationCreateInfo(VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    auto multisample = Info::Pipeline::MultisampleCreateInfo(VK_SAMPLE_COUNT_1_BIT);
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachment = {Info::Pipeline::ColorBlendAttachmentCreateInfo(VK_FALSE, BLEND_ONE, BLEND_ONE, BLEND_ZERO, BLEND_ZERO)};
    auto colorBlend = Info::Pipeline::ColorBlendCreateInfo(colorBlendAttachment);

    createDescriptor(device, uniformBuffers, swapchain.getImageCount());

    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;


    auto pipelineLayout = Info::Pipeline::LayoutCreateInfo({m_DescriptorLayout}, {});
    auto res = vkCreatePipelineLayout(device.get(), &pipelineLayout, nullptr, &m_Layout);

    VkGraphicsPipelineCreateInfo pipelineInfo {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInput,
            .pInputAssemblyState = &inputAssembly,
            .pTessellationState = nullptr,
            .pViewportState = &viewport,
            .pRasterizationState = &rasterization,
            .pMultisampleState = &multisample,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlend,
            .pDynamicState = nullptr,
            .layout = m_Layout,
            .renderPass = renderPass.get(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
    };
    auto pipelineRes = vkCreateGraphicsPipelines(device.get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);
}
VkPipeline                  ws::GraphicsPipeline::get() { return m_Pipeline; }
VkPipelineLayout            ws::GraphicsPipeline::getLayout() {return m_Layout;}
void                        ws::GraphicsPipeline::createDescriptor(ws::Device device, ws::UniformBuffer& buffers, u32 count) {

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    vkCreateDescriptorSetLayout(device.get(), &layoutInfo, nullptr, &m_DescriptorLayout);


    vector<VkDescriptorPoolSize> poolSize;
    poolSize.emplace_back(Info::DescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, count));
    VkDescriptorPoolCreateInfo poolInfo = Info::DescriptorPoolCreateInfo(count, poolSize);

    vkCreateDescriptorPool(device.get(), &poolInfo, nullptr, &m_Pool);



    vector<VkDescriptorSetLayout> Layouts(count, m_DescriptorLayout);

    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = m_Pool;
    allocateInfo.descriptorSetCount = count;
    allocateInfo.pSetLayouts = Layouts.data();
    m_DescriptorSets.resize(count);
    vkAllocateDescriptorSets(device.get(), &allocateInfo, m_DescriptorSets.data());
    for (u32 i = 0; i < count; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffers.get()[i];
        bufferInfo.offset = 0;
        bufferInfo.range = buffers.getSize();

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_DescriptorSets[i];
        write.descriptorCount = 1;
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo = &bufferInfo;


        // TODO: Update Descriptor Set Throws Error
        vkUpdateDescriptorSets(device.get(), 1, &write, 0, nullptr);
    }
}
vector<VkDescriptorSet>     ws::GraphicsPipeline::getDescriptorSet() {
    return m_DescriptorSets;
}

                            ws::Buffer::Buffer() {}
                            ws::Buffer::Buffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, VkBufferUsageFlags bufferType, void *bufferdata) {
    auto stagingBuffer = CreateBuffer(device.get(), device.getPhysicalDevice(), size,
                                      BUFFER_SENDER, RAM_MEMORY | COHERENT_MEMORY);


    void* data;
    vkMapMemory(device.get(), stagingBuffer.Memory, 0, size, 0, &data);
    memcpy(data, bufferdata, (size_t) size);
    vkUnmapMemory(device.get(), stagingBuffer.Memory);



    auto Buffer = CreateBuffer(device.get(), device.getPhysicalDevice(), size,
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferType,
                               GPU_MEMORY);

    CopyBuffer(device.get(), pool, device.getGraphicsQueue(), Buffer.Buffer, stagingBuffer.Buffer, size);
    m_Buffer = Buffer.Buffer;
    m_Memory = Buffer.Memory;

    vkFreeMemory(device.get(), stagingBuffer.Memory, nullptr);
    vkDestroyBuffer(device.get(), stagingBuffer.Buffer, nullptr);
}
void                        ws::Buffer::destroy(ws::Device device) {
    vkDestroyBuffer(device.get(), m_Buffer, nullptr);
    vkFreeMemory(device.get(), m_Memory, nullptr);
}
VkBuffer                    ws::Buffer::get() {
    return m_Buffer;
}

                            ws::VertexBuffer::VertexBuffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, void* vertexdata) : Buffer(device, pool, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexdata) {}


                            ws::IndexBuffer::IndexBuffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, void *indexdata) : Buffer(device, pool, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexdata) {}







