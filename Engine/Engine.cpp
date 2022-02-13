#include "Engine.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb-master/stb_image.h"

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
    auto StartTime = clock();
    Initalize();
    if (debug) addSetting(settings::Debug);
    m_Instance = CreateInstance();
    printf("Instance: %lims\n", clock()-StartTime);
}
VkInstance ws::Instance::get() { return m_Instance; }
const char* ws::Instance::getName() { return m_InstanceName; }


static void                 resizeCallback(GLFWwindow*, int, int) {

}
                            ws::Window::Window(ws::Instance& instance, uint32_t width, uint32_t height) {
    auto StartTime = clock();
    m_Window = glfwCreateWindow(width, height, instance.getName(), nullptr, nullptr);
    glfwCreateWindowSurface(instance.get(), m_Window, nullptr, &m_Surface);
    glfwSetFramebufferSizeCallback(m_Window, resizeCallback);
    printf("Window: %lims\n", clock()-StartTime);
}
bool                        ws::Window::isOpen() { return !glfwWindowShouldClose(m_Window); }
void                        ws::Window::swapBuffers() { glfwSwapBuffers(m_Window); }
void                        ws::Window::pollEvents() { glfwPollEvents(); }
GLFWwindow*                 ws::Window::get() { return m_Window; }
VkSurfaceKHR                ws::Window::getSurface() { return m_Surface; }
VkExtent2D                  ws::Window::size() const {
    VkExtent2D extent;
    glfwGetFramebufferSize(m_Window, reinterpret_cast<int *>(&extent.width), reinterpret_cast<int *>(&extent.height));
    return extent;
}
v2<u32>                     ws::Window::getMousePos() {
    double x,y;
    glfwGetCursorPos(m_Window, &x, &y);
    return {(u32)x,(u32)y};
}
bool                        ws::Window::keyDown(int keycode) {
    return glfwGetKey(m_Window, keycode);
}

VkPhysicalDevice                FindPhysicalDevice(VkInstance instance) {
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
bool                            deviceSupportsSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, u32 index) {
    VkBool32 support;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, surface, &support);
    return support;
}

                            ws::Device::Device(ws::Instance& instance, Window& window) {
    auto StartTime = clock();
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

    VkPhysicalDeviceFeatures features{};
    features.samplerAnisotropy = VK_TRUE;
    auto deviceInfo = Info::DeviceCreateInfo(qInfos, {}, extensions, &features);
    vkCreateDevice(m_PhysicalDevice, &deviceInfo, nullptr, &m_Device);

    vkGetDeviceQueue(m_Device, m_GraphicsIndex, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, m_PresentIndex, 0, &m_PresentQueue);
    printf("Device: %lims\n", clock()-StartTime);
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
vector<VkSemaphore>         ws::Device::createSemaphores(uint32_t count) {
    vector<VkSemaphore> semaphores(count, nullptr);
    for (auto& i : semaphores) {
        auto info = Info::SemaphoreCreateInfo();
        auto res = vkCreateSemaphore(m_Device, &info, nullptr, &i);
    }
    return semaphores;
}
vector<VkFence>             ws::Device::createFences(uint32_t count) {
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
ws::ImageArray              CreateSwapchainImagesAndViews(VkDevice device, VkSwapchainKHR swapchain, VkFormat format) {
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
void                        ws::Swapchain::create(ws::Device device, ws::Window window) {
    auto StartTime = clock();
    m_Formats.Extent = {window.size().width, window.size().height};
    m_Formats.Format = getSurfaceFormat(device.getPhysicalDevice(), window.getSurface());
    m_Formats.PresentMode = getPresentMode(device.getPhysicalDevice(), window.getSurface());
    auto capa = Info::GetSurfaceCapabilities(device.getPhysicalDevice(), window.getSurface());
    u32 minImageCount = getMinimumImageCount(device.getPhysicalDevice(), window.getSurface());
    vector<u32> queueIndices = { device.getGraphicsIndex() };

    auto createInfo = Info::SwapchainCreateInfo(m_Formats, window.getSurface(), minImageCount, capa.currentTransform, queueIndices);

    vkCreateSwapchainKHR(device.get(), &createInfo, nullptr, &m_Swapchain);
    m_Images = CreateSwapchainImagesAndViews(device.get(), m_Swapchain, m_Formats.Format.format);
    printf("Swapchain: %lims\n", clock()-StartTime);
}
                            ws::Swapchain::Swapchain(ws::Device device, ws::Window window) {
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
u32                         ws::Swapchain::getImageIndex(ws::Device device, VkSemaphore& semaphore, Result* result) {
    u32 index = 0;
    *result = vkAcquireNextImageKHR(device.get(), m_Swapchain, UINT64_MAX, semaphore, nullptr, &index);
    return index;
}
void                        ws::Swapchain::destroy(ws::Device device) {
    for (auto& i : m_Framebuffers)
        vkDestroyFramebuffer(device.get(), i, nullptr);
    for (auto& i : m_Images.View)
        vkDestroyImageView(device.get(), i, nullptr);
    vkDestroySwapchainKHR(device.get(), m_Swapchain, nullptr);
}
void                        ws::Swapchain::recreate(ws::Device device, ws::Window window) {
    create(device, window);
}

                            ws::CommandBuffer::CommandBuffer(ws::Device &device, VkCommandPool* owner, uint32_t count) : m_OwnerPool(owner), m_Index(0) {
    auto StartTime = clock();
    m_Buffers.resize(count);
    auto info = Info::CommandBufferAllocationInfo(*m_OwnerPool, count, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    auto res = vkAllocateCommandBuffers(device.get(), &info, m_Buffers.data());
    printf("Command Buffers: %lims\n", clock()-StartTime);
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


                            ws::CommandPool::CommandPool(ws::Device device) {
    auto StartTime = clock();
    auto createInfo = Info::CommandPoolCreateInfo(device.getGraphicsIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    vkCreateCommandPool(device.get(), &createInfo, nullptr, &m_Pool);
    printf("Command Pool: %lims\n", clock()-StartTime);
}
                            ws::CommandBuffer ws::CommandPool::AllocateBuffers(ws::Device& device, u32 count) {
    auto buffers = CommandBuffer(device, &m_Pool, count);
    m_Buffers = &buffers;
    return buffers;
}
VkCommandPool               ws::CommandPool::get() {
    return m_Pool;
}


void                        ws::RenderPass::create(ws::Device device, ws::Swapchain swapchain) {
    auto StartTime = clock();
    std::vector<VkAttachmentDescription> colorAttchment = {Info::RenderPassAttachmentCreateInfo(swapchain.getFormats().Format.format, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)};
    auto colorAttachmentRef = Info::RenderPassAttachmentReferenceCreateInfo(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    std::vector<VkSubpassDescription> subpass = {Info::SubpassDescriptionCreateInfo(VK_PIPELINE_BIND_POINT_GRAPHICS, colorAttachmentRef)};
    std::vector<VkSubpassDependency> dependancy = {Info::SubpassDependencyCreateInfo(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)};
    auto info = Info::RenderPassCreateInfo(colorAttchment, subpass, dependancy);

    VkResult renderpassResult = vkCreateRenderPass(device.get(), &info, nullptr, &m_RenderPass);
    printf("Render Pass: %lims\n", clock()-StartTime);
}
                            ws::RenderPass::RenderPass(ws::Device device, ws::Swapchain swapchain) {
    create(device, swapchain);
}
VkRenderPass                ws::RenderPass::get() const { return m_RenderPass; }
void                        ws::RenderPass::destroy(ws::Device device) {
    vkDestroyRenderPass(device.get(), m_RenderPass, nullptr);
}
void                        ws::RenderPass::recreate(ws::Device device, ws::Swapchain swapchain) {
    create(device, swapchain);
}


void                        ws::UniformBuffer::create(ws::Device device, VkCommandPool pool, VkDeviceSize size, uint32_t count) {
    auto StartTime = clock();
    m_Buffer.resize(count);
    m_Memory.resize(count);
    m_Size = size;
    for (int i = 0; i < m_Buffer.size(); i++) {
        auto buffer = CreateBuffer(device.get(), device.getPhysicalDevice(), size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, RAM_MEMORY | COHERENT_MEMORY);
        m_Buffer[i] = buffer.Buffer;
        m_Memory[i] = buffer.Memory;
    }
    printf("Uniform Buffers: %lims\n", clock()-StartTime);
}
                            ws::UniformBuffer::UniformBuffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, u32 count) {
    create(device, pool, size, count);
}
vector<VkDeviceMemory>      ws::UniformBuffer::getMem() {
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
void                        ws::UniformBuffer::recreate(ws::Device device, VkCommandPool pool, VkDeviceSize size, uint32_t count) {
    create(device, pool, size, count);
}



void ws::PipelineFactory::addShaderComponent(Shader shader) {
    m_ShaderStages.emplace_back(shader);
}
void ws::PipelineFactory::addShaderComponent(std::initializer_list<Shader> shaders) {
    for (auto i : shaders) {
        m_ShaderStages.emplace_back(i);
    }
}
void ws::PipelineFactory::addVertexInputComponent(VertexInputData vertexData) {
    m_VertexInfo = Info::Pipeline::VertexInputCreateInfo(vertexData.bindings, vertexData.attributes);
}
void ws::PipelineFactory::addInputAssemblyComponent(VkPrimitiveTopology topology) {
    m_InputAssembly = Info::Pipeline::InputAssemblyCreateInfo(topology);
}
void ws::PipelineFactory::addViewportComponent(VkExtent2D extent) {
    m_Viewport = Info::Viewport(0.0f,0.0f, extent);
    m_Scissor = {{0,0}, extent};
}
void ws::PipelineFactory::addViewportComponent(Window window) {
    addViewportComponent(window.size());
}
void ws::PipelineFactory::addRasterizationComponent(VkPolygonMode drawMode, VkCullModeFlags cullmode, VkFrontFace frontface) {
    m_Rasterization = Info::Pipeline::RasterizationCreateInfo(VK_FALSE, drawMode, cullmode, frontface);
}
void ws::PipelineFactory::addMultiSampleComponent(VkSampleCountFlagBits samples) {
    m_Multisample = Info::Pipeline::MultisampleCreateInfo(samples);
}
void ws::PipelineFactory::addColorBlendComponent(vector<VkPipelineColorBlendAttachmentState> attachments) {
    colorBlendAttachment.resize(attachments.size());
    if (attachments.empty()) {
        colorBlendAttachment.resize(1);
        colorBlendAttachment[0] = Info::Pipeline::ColorBlendAttachmentCreateInfo(VK_FALSE, BLEND_ONE, BLEND_ONE, BLEND_ZERO, BLEND_ZERO);
    } else {
        colorBlendAttachment = attachments;
    }
}
ws::DescriptorObject* ws::PipelineFactory::addDescriptorObjects(vector<DescriptorSet> bindings, ws::Device device) {
    u32 maxSets = 0;
    vector<VkDescriptorSetLayoutBinding> layoutBindings(bindings.size());
    vector<VkDescriptorPoolSize> poolSizes(bindings.size());
    for (int i = 0; i < bindings.size(); i++) {
        layoutBindings[i] = Info::DescriptorLayoutBinding(i, 1, bindings[i].Type, bindings[i].Access);
        poolSizes[i] = Info::DescriptorPoolSize(bindings[i].Type, bindings[i].Count);
        if (bindings[i].Count > maxSets) maxSets = bindings[i].Count;
    }
    VkDescriptorSetLayoutCreateInfo layoutInfo = Info::DescriptorLayoutCreateInfo(layoutBindings);
    vkCreateDescriptorSetLayout(device.get(), &layoutInfo, VK_NULL_HANDLE, &m_Descriptor.Layout);
    VkDescriptorPoolCreateInfo poolInfo = Info::DescriptorPoolCreateInfo(maxSets, poolSizes);
    vkCreateDescriptorPool(device.get(), &poolInfo, VK_NULL_HANDLE, &m_Descriptor.Pool);

    vector<VkDescriptorSetLayout> layouts(maxSets, m_Descriptor.Layout);
    m_Descriptor.Sets.resize(maxSets);
    VkDescriptorSetAllocateInfo allocInfo = Info::DescriptorAllocInfo(layouts, m_Descriptor.Pool, maxSets);
    vkAllocateDescriptorSets(device.get(), &allocInfo, m_Descriptor.Sets.data());
    for (auto &binding : bindings) {
        for (u32 i = 0; i < maxSets; i++) {
            auto write = Info::DescriptorWrite(binding.Type, 1, binding.DescriptorWrite[i].binding, m_Descriptor.Sets[i]);
            if (binding.DescriptorWrite[i].bufferInfo.has_value()) {
                write.pBufferInfo = &binding.DescriptorWrite[i].bufferInfo.value();
            }
            if (binding.DescriptorWrite[i].imageInfo.has_value()) {
                write.pImageInfo = &binding.DescriptorWrite[i].imageInfo.value();
            }
            vkUpdateDescriptorSets(device.get(), 1, &write, 0, VK_NULL_HANDLE);
        }
    }
    DescriptorObject* descriptor = &m_Descriptor;
    return descriptor;
}
ws::PipelineObject* ws::PipelineFactory::build(ws::Device device, ws::RenderPass renderPass) {

    VkPipelineViewportStateCreateInfo m_ViewportState = Info::Pipeline::Viewport(m_Viewport, m_Scissor);
    VkPipelineColorBlendStateCreateInfo m_ColorBlend = Info::Pipeline::ColorBlendCreateInfo(colorBlendAttachment);

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages(m_ShaderStages.size());
    for (int i = 0; i < shaderStages.size(); i++) {
        shaderStages[i] = Info::Pipeline::ShaderStageCreateInfo(m_ShaderStages[i]);
    }


    std::vector<VkDescriptorSetLayout> descriptorLayouts = {m_Descriptor.Layout};
    auto layoutInfo = Info::Pipeline::LayoutCreateInfo(descriptorLayouts, m_PushConstants);
    vkCreatePipelineLayout(device.get(), &layoutInfo, VK_NULL_HANDLE, &m_Pipelines.Layout);

    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.flags = 0;
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &m_VertexInfo;
    pipelineInfo.pInputAssemblyState = &m_InputAssembly;
    pipelineInfo.pTessellationState = nullptr;
    pipelineInfo.pViewportState = &m_ViewportState;
    pipelineInfo.pRasterizationState = &m_Rasterization;
    pipelineInfo.pMultisampleState = &m_Multisample;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &m_ColorBlend;
    pipelineInfo.pDynamicState = nullptr;
    pipelineInfo.layout = m_Pipelines.Layout;
    pipelineInfo.renderPass = renderPass.get();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    vkCreateGraphicsPipelines(device.get(), VK_NULL_HANDLE, 1, &pipelineInfo, VK_NULL_HANDLE, &m_Pipelines.Pipeline);

    return &m_Pipelines;
}


ws::PipelineFactory pipelineFactory;


void                        ws::GraphicsPipeline::create(GraphicsPipelineConstructor) {
    auto StartTime = clock();

    ws::DescriptorSet descriptor;
    descriptor.Count = swapchain.getImageCount();
    descriptor.Type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor.Access = VERTEX_SHADER;
    for (u32 i = 0; i < swapchain.getImageCount(); i++) {
        ws::WriteInfo info;
        info.binding = 0;
        info.bufferInfo = Info::DescriptorBufferInfo(uniformBuffers.get()[0], uniformBuffers.getSize(), 0);
        descriptor.DescriptorWrite.emplace_back(info);
    }

    pipelineFactory.addShaderComponent({vShader, fShader});
    pipelineFactory.addVertexInputComponent(inputData);
    pipelineFactory.addInputAssemblyComponent(TOPOLOGY_TRIANGLE_LIST);
    pipelineFactory.addMultiSampleComponent(VK_SAMPLE_COUNT_1_BIT);
    pipelineFactory.addColorBlendComponent();
    pipelineFactory.addRasterizationComponent(VK_POLYGON_MODE_FILL);
    pipelineFactory.addViewportComponent(swapchain.getExtent());
    auto descriptors = pipelineFactory.addDescriptorObjects({descriptor}, device);
    auto pipeline = pipelineFactory.build(device, renderPass);

    m_Pipeline = pipeline->Pipeline;
    m_Layout = pipeline->Layout;
    m_DescriptorLayout = descriptors->Layout;
    m_Pool = descriptors->Pool;
    m_DescriptorSets = descriptors->Sets;


    printf("Pipeline: %lims\n", clock()-StartTime);
}
                            ws::GraphicsPipeline::GraphicsPipeline(GraphicsPipelineConstructor) {
    create(device, swapchain, renderPass, vShader, fShader, inputData, uniformBuffers);
}
VkPipeline                  ws::GraphicsPipeline::get() { return m_Pipeline; }
VkPipelineLayout            ws::GraphicsPipeline::getLayout() {return m_Layout;}
void                        ws::GraphicsPipeline::createDescriptor(ws::Device device, ws::UniformBuffer& buffers, u32 count) {



    VkDescriptorSetLayoutBinding uboLayoutBinding = Info::DescriptorLayoutBinding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VERTEX_SHADER);
    VkDescriptorSetLayoutBinding samplerLayoutBinding = Info::DescriptorLayoutBinding(1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, FRAGMENT_SHADER);
    std::array<VkDescriptorSetLayoutBinding, 2> descriptorBindings = {uboLayoutBinding, samplerLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = descriptorBindings.size();
    layoutInfo.pBindings = descriptorBindings.data();

    vkCreateDescriptorSetLayout(device.get(), &layoutInfo, nullptr, &m_DescriptorLayout);


    vector<VkDescriptorPoolSize> poolSize;
    poolSize.emplace_back(Info::DescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, count));
    poolSize.emplace_back(Info::DescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, count));
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

        /*VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_Textures[0].View;
        imageInfo.sampler = m_Sampler;*/

        VkWriteDescriptorSet write[1]{};
        write[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write[0].dstSet = m_DescriptorSets[i];
        write[0].descriptorCount = 1;
        write[0].dstBinding = 0;
        write[0].dstArrayElement = 0;
        write[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write[0].pBufferInfo = &bufferInfo;

        /*write[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write[1].dstSet = m_DescriptorSets[i];
        write[1].dstBinding = 1;
        write[1].dstArrayElement = 0;
        write[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write[1].descriptorCount = 1;
        write[1].pImageInfo = &imageInfo;*/


        // TODO: Update Descriptor Set Throws Error
        vkUpdateDescriptorSets(device.get(), 1, write, 0, nullptr);
    }
}
vector<VkDescriptorSet>     ws::GraphicsPipeline::getDescriptorSet() {
    return m_DescriptorSets;
}
void                        ws::GraphicsPipeline::createSampler(ws::Device device) {

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(device.getPhysicalDevice(), &properties);
    VkSamplerCreateInfo samplerInfo = Info::SamplerCreateInfo(properties.limits.maxSamplerAnisotropy);
    auto res = vkCreateSampler(device.get(), &samplerInfo, nullptr, &m_Sampler);

}
/*
void                        ws::GraphicsPipeline::expectTextures(uint32_t count) {

}
void                        ws::GraphicsPipeline::loadTexture(ws::Texture texture, const char *name) {
    if (name == nullptr) return;
    texture.handle = name;
    m_Textures.emplace_back(texture);


}
void                        ws::GraphicsPipeline::unloadTexture(const char *name) {
    for (int i = 0; i < m_Textures.size(); i++) {
        if (m_Textures[i].handle == name) {
            m_Textures.erase(m_Textures.begin()+i);
            break;
        }
    }
}
*/
void                        ws::GraphicsPipeline::destroy(ws::Device device) {
    vkDestroyPipeline(device.get(), m_Pipeline, nullptr);
    vkDestroyPipelineLayout(device.get(), m_Layout, nullptr);
    vkDestroyDescriptorPool(device.get(), m_Pool, nullptr);
}
void                        ws::GraphicsPipeline::recreate(GraphicsPipelineConstructor) {
    create(device, swapchain, renderPass, vShader, fShader, inputData, uniformBuffers);
}


void                        ws::Descriptor::createLayout(ws::Device device, vector<VkDescriptorSetLayoutBinding> bindings) {

    auto layoutInfo = Info::DescriptorLayoutCreateInfo(bindings);
    for (const auto i : bindings) {
        m_Sizes.emplace_back(Info::DescriptorPoolSize(i.descriptorType, 0));
    }
    vkCreateDescriptorSetLayout(device.get(), &layoutInfo, nullptr, &m_Layout);

}
void                        ws::Descriptor::createPool(ws::Device device, const u32 descriptorCount[]) {
    u32 maxSets = 0;
    for (u32 i = 0; i < m_Sizes.size(); i++) {
        if (descriptorCount[i] > maxSets) maxSets = descriptorCount[i];
        m_Sizes[i].descriptorCount = descriptorCount[i];
    }
    auto poolInfo = Info::DescriptorPoolCreateInfo(maxSets, m_Sizes);
    vkCreateDescriptorPool(device.get(), &poolInfo, nullptr, &m_Pool);
}
void                        ws::Descriptor::allocate(ws::Device device, uint32_t count) {
    vector<VkDescriptorSetLayout> layouts(count, m_Layout);
    auto allocinfo = Info::DescriptorAllocInfo(layouts, m_Pool, count);
    vkAllocateDescriptorSets(device.get(), &allocinfo, m_Sets.data());
}
//void                        ws::Descriptor::create(ws::Device device) {
//    vector<VkWriteDescriptorSet> write;
    //Info::DescriptorWrite();
  //  vkUpdateDescriptorSets(device.get(), write.size(), write.data(), 0, nullptr);
//}













                            ws::Buffer::Buffer() {}
                            ws::Buffer::Buffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, VkBufferUsageFlags bufferType, void *bufferdata) {
    auto StartTime = clock();
    auto stagingBuffer = CREATE_STAGING_BUFFER(device.get(), device.getPhysicalDevice(), size);


    UPLOAD_BUFFER_TO_MEMORY(device.get(), stagingBuffer.Memory, bufferdata, size);



    auto Buffer = CreateBuffer(device.get(), device.getPhysicalDevice(), size,
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferType,
                               GPU_MEMORY);

    CopyBuffer(device.get(), pool, device.getGraphicsQueue(), Buffer.Buffer, stagingBuffer.Buffer, size);
    m_Buffer = Buffer.Buffer;
    m_Memory = Buffer.Memory;

    vkFreeMemory(device.get(), stagingBuffer.Memory, nullptr);
    vkDestroyBuffer(device.get(), stagingBuffer.Buffer, nullptr);
    printf("Buffer: %lims\n", clock()-StartTime);
}
void                        ws::Buffer::destroy(ws::Device device) {
    vkDestroyBuffer(device.get(), m_Buffer, nullptr);
    vkFreeMemory(device.get(), m_Memory, nullptr);
}
VkBuffer                    ws::Buffer::get() {
    return m_Buffer;
}

                            ws::VertexBuffer::VertexBuffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, void* vertexdata) {
    auto StartTime = clock();
    auto stagingBuffer = CREATE_STAGING_BUFFER(device.get(), device.getPhysicalDevice(), size);
    m_StagingBuffer = stagingBuffer.Buffer;
    m_StagingMemory = stagingBuffer.Memory;

    UPLOAD_BUFFER_TO_MEMORY(device.get(), stagingBuffer.Memory, vertexdata, size);



    auto Buffer = CreateBuffer(device.get(), device.getPhysicalDevice(), size,
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                               GPU_MEMORY);

    CopyBuffer(device.get(), pool, device.getGraphicsQueue(), Buffer.Buffer, stagingBuffer.Buffer, size);
    m_Buffer = Buffer.Buffer;
    m_Memory = Buffer.Memory;


    printf("Buffer: %lims\n", clock()-StartTime);
}
void                        ws::VertexBuffer::upload(ws::Device device, VkCommandPool pool, VkDeviceSize size, void* vertexdata) {
    auto StartTime = clock();
    destroy(device);

    auto tempBuffer = CREATE_STAGING_BUFFER(device.get(), device.getPhysicalDevice(), size);
    m_StagingBuffer = tempBuffer.Buffer;
    m_StagingMemory = tempBuffer.Memory;

    UPLOAD_BUFFER_TO_MEMORY(device.get(), m_StagingMemory, vertexdata, size);

    auto tempBufferVertex = CreateBuffer(device.get(), device.getPhysicalDevice(),size, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, GPU_MEMORY);
    m_Buffer = tempBufferVertex.Buffer;
    m_Memory = tempBufferVertex.Memory;

    CopyBuffer(device.get(), pool, device.getGraphicsQueue(), m_Buffer, m_StagingBuffer, size);
    printf("Vertex Buffer Recreation: %lims\n", clock()-StartTime);
}
VkBuffer                    ws::VertexBuffer::get() { return m_Buffer; }
void                        ws::VertexBuffer::destroy(ws::Device device) {
    vkFreeMemory(device.get(), m_StagingMemory, nullptr);
    vkDestroyBuffer(device.get(), m_StagingBuffer, nullptr);
    vkDestroyBuffer(device.get(), m_Buffer, nullptr);
    vkFreeMemory(device.get(), m_Memory, nullptr);
}

                            ws::IndexBuffer::IndexBuffer(ws::Device device, VkCommandPool pool, VkDeviceSize size, void *indexdata) : Buffer(device, pool, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexdata) {}


VkCommandBuffer beginSingleTimeCommands(ws::Device device, VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device.get(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}
void endSingleTimeCommands(ws::Device device, VkQueue queue, VkCommandPool commandPool, VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device.get(), commandPool, 1, &commandBuffer);
}


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








