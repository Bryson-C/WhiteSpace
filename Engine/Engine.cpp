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

Result Initalize() {
    if (!glfwInit()) return ERROR;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
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
    vec<VkClearValue> clearColor = {{0.02f,0.02f,0.02f,1.0f}};
    auto info = Info::RenderPassBeginInfo(clearColor, extent, Framebuffer, renderPass);
    vkCmdBeginRenderPass(commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    return SUCCESS;
}

Result EndRenderPass(VkCommandBuffer& Commandbuffer) { vkCmdEndRenderPass(Commandbuffer); return SUCCESS; }

std::vector<VkFence> CreateFence(VkDevice device, const u32 count) {
    vec<VkFence> fences(count);
    for (auto& i : fences) {
        auto info = Info::FenceCreateInfo();
        auto res = vkCreateFence(device, &info, nullptr, &i);
    }
    return fences;
}

std::vector<VkSemaphore> CreateSemaphore(VkDevice device, const u32 count) {
    vec<VkSemaphore> semaphores(count, nullptr);
    for (auto& i : semaphores) {
        auto info = Info::SemaphoreCreateInfo();
        auto res = vkCreateSemaphore(device, &info, nullptr, &i);
    }
    return semaphores;
}

/*
Result CreateWindow(Window& window, int w, int h, std::string name, Instance instance) {
    name += " ["; name += EngineName; name += "]";
    if (settingDefined(settings::NoWindowHeader)) glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    window.Window = glfwCreateWindow(w, h, name.c_str(), nullptr, nullptr);
    if (settingDefined(settings::NoWindowHeader))    glfwSetWindowPos(window.Window, 100, 100);
    if (window.Window == nullptr) return ERROR;
    auto res = glfwCreateWindowSurface(instance.Instance, window.Window, nullptr, &window.Surface);
    if (res != VK_SUCCESS) return res;
    return SUCCESS;
}
Window CreateWindow(int w, int h, std::string name, Instance instance) {
    Window window{};
    auto res = CreateWindow(window, w, h, name, instance);
    return window;
}




/// TODO: Fix Device Creation
Result CreateDevice(Instance instance, Device& device, Window window) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.Instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance.Instance, &deviceCount, devices.data());
    device.PhysicalDevice = devices[0]; // iterate through devices later


    uint32_t qCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device.PhysicalDevice, &qCount, nullptr);
    std::vector<VkQueueFamilyProperties> qProperties(qCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device.PhysicalDevice, &qCount, qProperties.data());

    bool useGraphics = false;
    bool useTransfer = false;
    for (uint32_t i = 0; i < qProperties.size(); i++) {
        if (qProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            device.Graphics.Index = i;
            useGraphics = true;
        }

        VkBool32 presentSupport;
        vkGetPhysicalDeviceSurfaceSupportKHR(device.PhysicalDevice, i, window.Surface, &presentSupport);

        if (presentSupport) {
            device.Transfer.Index = i;
            useTransfer = true;
        }
    }

    const float priority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> QueueInfo{};
    if (useGraphics) QueueInfo.push_back(Info::QueueCreateInfo(device.Graphics.Index, 1, &priority));
    if (useTransfer) QueueInfo.push_back(Info::QueueCreateInfo(device.Transfer.Index, 1, &priority));

    VkPhysicalDeviceFeatures features{};
    std::vector<const char*> extensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    auto deviceInfo = Info::DeviceCreateInfo(QueueInfo, {}, extensions, &features);
    auto res = vkCreateDevice(device.PhysicalDevice, &deviceInfo, nullptr, &device.Device);

    if (res != VK_SUCCESS) return ERROR;


    if (useGraphics) vkGetDeviceQueue(device.Device, device.Graphics.Index, 0, &device.Graphics.Queue);
    if (useTransfer) vkGetDeviceQueue(device.Device, device.Transfer.Index, 0, &device.Graphics.Queue);

    return res;
}
Device CreateDevice(Instance instance, Window window) {
    Device device{};
    auto res = CreateDevice(instance, device, window);
    return device;
}

Result CreateDescriptor(Device device, Descriptor& descriptor, const std::vector<VkDescriptorPoolSize>& poolSize) {
    auto info = Info::DescriptorPoolCreateInfo(1, poolSize);
    auto res = vkCreateDescriptorPool(device.Device, &info, nullptr, &descriptor.Pool);
    if (res != VK_SUCCESS) return ERROR;
    return res;
}

Result CreateSwapchain(Device device, Window window, Swapchain& swapchain) {
    auto presentationInfo = Info::GetPresentationInformation(device, window);
    auto capabilities = Info::GetSurfaceCapabilities(device, window);
    uint32_t swapchainImageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && swapchainImageCount > capabilities.maxImageCount) swapchainImageCount = capabilities.maxImageCount;

    swapchain.formats = presentationInfo;
    swapchain.viewport = Info::Viewport(0.0f,0.0f, swapchain.formats.Extent, 0.0f, 1.0f);
    swapchain.scissor = {{0,0},swapchain.formats.Extent};

    VkBool32 support;
    vkGetPhysicalDeviceSurfaceSupportKHR(device.PhysicalDevice, device.Transfer.Index, window.Surface, &support);
    if (!support) vkGetPhysicalDeviceSurfaceSupportKHR(device.PhysicalDevice, device.Graphics.Index, window.Surface, &support);


    std::vector<u32> queueIndices = {};
    if (device.Graphics.initialized) queueIndices.push_back(device.Graphics.Index);
    if (device.Transfer.initialized) queueIndices.push_back(device.Transfer.Index);


    auto info = Info::SwapchainCreateInfo(swapchain.formats, window, swapchainImageCount, capabilities.currentTransform, queueIndices);
    auto res = vkCreateSwapchainKHR(device.Device, &info, nullptr, &swapchain.swapchain);
    if (res != SUCCESS) return res;

    auto imgres = CreateSwapchainImagesAndViews(device, swapchain);
    if (imgres != SUCCESS) return imgres;

    return SUCCESS;
}
Swapchain CreateSwapchain(Device device, Window window) {
    Swapchain swapchain{};
    auto res = CreateSwapchain(device, window, swapchain);
    return swapchain;
}

Result CreateShader(Device device, const std::string& path, Shader& shader, VkShaderStageFlagBits type) {
    shader.Type = type;
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (file.is_open()) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        //auto info = Info::ShaderModuleCreateInfo(buffer);
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = buffer.size();
        info.pCode = reinterpret_cast<uint32_t*>(buffer.data());
        auto res = vkCreateShaderModule(device.Device, &info, nullptr, &shader.Module);
        if (res != SUCCESS) return res;
        return SUCCESS;
    }
    return ERROR;
}
Shader CreateShader(Device device, const std::string& path, VkShaderStageFlagBits type) {
    Shader shader{};
    auto res = CreateShader(device, path, shader, type);
    return shader;
}


Result CreateRenderPass(Device device, const Swapchain& swapchain, VkRenderPass& renderPass) {

    std::vector<VkAttachmentDescription> colorAttchment = {Info::RenderPassAttachmentCreateInfo(swapchain.formats.Format.format, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)};
    auto colorAttachmentRef = Info::RenderPassAttachmentReferenceCreateInfo(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    std::vector<VkSubpassDescription> subpass = {Info::SubpassDescriptionCreateInfo(VK_PIPELINE_BIND_POINT_GRAPHICS, colorAttachmentRef)};
    std::vector<VkSubpassDependency> dependancy = {Info::SubpassDependencyCreateInfo(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)};
    auto info = Info::RenderPassCreateInfo(colorAttchment, subpass, dependancy);

    VkResult renderpassResult = vkCreateRenderPass(device.Device, &info, nullptr, &renderPass);
    if (renderpassResult != VK_SUCCESS) return renderpassResult;
    return SUCCESS;
}
VkRenderPass CreateRenderPass(Device device, Swapchain& swapchain) {
    VkRenderPass renderPass{};
    auto res = CreateRenderPass(device, swapchain, renderPass);
    return renderPass;
}

Result CreatePipeline(Device device, const Swapchain& swapchain, VkRenderPass& renderPass, Shader vertex, Shader fragment, Pipeline& pipeline) {
    auto vertexInfo = Info::Pipeline::ShaderStageCreateInfo(vertex);
    auto fragmentInfo = Info::Pipeline::ShaderStageCreateInfo(fragment);
    VkPipelineShaderStageCreateInfo shaderStages[2] = {vertexInfo,fragmentInfo};

    auto vertexInput = Info::Pipeline::VertexInputCreateInfo({},{});
    auto inputAssembly = Info::Pipeline::InputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    auto viewport = Info::Pipeline::Viewport(swapchain);
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
    auto res = vkCreatePipelineLayout(device.Device, &pipelineLayout, nullptr, &pipeline.Layout);
    if (res != SUCCESS) return res;

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
            .layout = pipeline.Layout,
            .renderPass = renderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
    };
    auto pipelineRes = vkCreateGraphicsPipelines(device.Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.Pipeline);
    if (pipelineRes != SUCCESS) return pipelineRes;
    return SUCCESS;
}
Pipeline CreatePipeline(Device device, Swapchain swapchain, VkRenderPass renderPass, Shader vertex, Shader fragment) {
    Pipeline pipeline{};
    auto res = CreatePipeline(device, swapchain, renderPass, vertex, fragment, pipeline);
    return pipeline;
}

Result CreateFramebuffers(Device device, Swapchain& swapchain, VkRenderPass renderPass) {
    swapchain.framebuffers.resize(swapchain.imageViews.size());
    for (int i = 0; i < swapchain.framebuffers.size(); i++) {
        std::vector<VkImageView> attachments = {swapchain.imageViews[i]};
        auto info = Info::FramebufferCreateInfo(renderPass, attachments, swapchain.formats.Extent, 1);
        auto res = vkCreateFramebuffer(device.Device, &info, nullptr, &swapchain.framebuffers[i]);
        if (res != SUCCESS) return res;
    }
    return SUCCESS;
}

Result CreateCommandPool(Device device, Queue queue, CommandPool& pool) {
    auto info = Info::CommandPoolCreateInfo(queue);
    auto res = vkCreateCommandPool(device.Device, &info, nullptr, &pool.Pool);
    if (res != VK_SUCCESS) return ERROR;
    return res;
}
CommandPool CreateCommandPool(Device device, Queue queue) {
    CommandPool pool{};
    auto res = CreateCommandPool(device, queue, pool);
    return pool;
}

Result CreateCommandBuffer(Device device ,CommandPool& pool, u32 count) {
    pool.Buffers.resize(count);
    auto info = Info::CommandBufferAllocationInfo(pool.Pool, count, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    auto res = vkAllocateCommandBuffers(device.Device, &info, pool.Buffers.data());
    if (res != SUCCESS) return res;
    return SUCCESS;
}

Result BeginRenderPass(Swapchain swapchain, VkRenderPass& renderPass, VkFramebuffer& Framebuffer, VkCommandBuffer& commandBuffer) {
    VkClearValue clearColor = {{0.02f,0.02f,0.02f,1.0f}};
    auto info = Info::RenderPassBeginInfo({clearColor}, swapchain.formats.Extent, Framebuffer, renderPass);
    vkCmdBeginRenderPass(commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    return SUCCESS;
}

Result CreateSemaphore(Device device, std::vector<VkSemaphore>& semaphores, const u32 count) {
    semaphores.resize(count);
    for (auto& i : semaphores) {
        auto info = Info::SemaphoreCreateInfo();
        auto res = vkCreateSemaphore(device.Device, &info, nullptr, &i);
        if (res != SUCCESS) return res;
    }
    return SUCCESS;
}
std::vector<VkSemaphore> CreateSemaphore(Device device, const u32 count) {
    std::vector<VkSemaphore> semaphores{};
    auto res = CreateSemaphore(device, semaphores, count);
    return semaphores;
}

Result CreateFence(Device device, std::vector<VkFence>& fence, const u32 count) {
    fence.resize(count);
    for (auto& i : fence) {
        auto info = Info::FenceCreateInfo();
        auto res = vkCreateFence(device.Device, &info, nullptr, &i);
        if (res != SUCCESS) return res;
    }
    return SUCCESS;
}
std::vector<VkFence> CreateFence(Device device, const u32 count) {
    std::vector<VkFence> fence{};
    auto res = CreateFence(device, fence, count);
    return fence;
}

*/

$::Instance::Instance(const char* name, bool debug) : m_InstanceName(name), m_Debug(debug) {
    Initalize();
    if (debug) addSetting(settings::Debug);
    m_Instance = CreateInstance();
}
VkInstance $::Instance::get() { return m_Instance; }
const char* $::Instance::getName() { return m_InstanceName; }

$::Window::Window($::Instance& instance, uint32_t width, uint32_t height) {
    m_Window = glfwCreateWindow(width, height, instance.getName(), nullptr, nullptr);
    glfwCreateWindowSurface(instance.get(), m_Window, nullptr, &m_Surface);
    glfwSetFramebufferSizeCallback(m_Window, resizeCallbacK);
}
bool $::Window::isOpen() { return !glfwWindowShouldClose(m_Window); }
void $::Window::swapBuffers() { glfwSwapBuffers(m_Window); }
void $::Window::pollEvents() { glfwPollEvents(); }
GLFWwindow* $::Window::get() { return m_Window; }
VkSurfaceKHR $::Window::getSurface() { return m_Surface; }
v2<uint32_t> $::Window::size() const {
    int w,h;
    glfwGetFramebufferSize(m_Window, &w, &h);
    return {static_cast<u32>(w),static_cast<u32>(h)};
}
void $::Window::resizeCallbacK(GLFWwindow *window, int w, int h) {}

VkPhysicalDevice FindPhysicalDevice(VkInstance instance) {
    u32 count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    vec<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(instance, &count, devices.data());
    for (auto i : devices) {
        return i;
    }
}
vec<VkQueueFamilyProperties> FindQueueFamilyProperties(VkPhysicalDevice physicalDevice) {
    u32 count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
    vec<VkQueueFamilyProperties> queueFamilies(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilies.data());
    return queueFamilies;
}
bool deviceSupportsSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, u32 index) {
    VkBool32 support;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, surface, &support);
    return support;
}

$::Device::Device($::Instance& instance, Window& window) {
    m_PhysicalDevice = FindPhysicalDevice(instance.get());
    auto queueFamilyProps = FindQueueFamilyProperties(m_PhysicalDevice);

    u32 loop = 0;
    for (auto& i : queueFamilyProps) {
        if (i.queueFlags & qGRAPHICS_FLAG) { m_GraphicsIndex = loop; }
        if (deviceSupportsSurface(m_PhysicalDevice, window.getSurface(), loop)) { m_PresentIndex = loop; }
        loop++;
    }
    const float priority = 1.0f;
    vec<VkDeviceQueueCreateInfo> qInfos{};
    qInfos.emplace_back(Info::QueueCreateInfo(m_GraphicsIndex, 1, &priority));
    if (m_PresentIndex != m_GraphicsIndex) qInfos.emplace_back(Info::QueueCreateInfo(m_PresentIndex, 1, &priority));
    vec<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    auto deviceInfo = Info::DeviceCreateInfo(qInfos, {}, extensions);
    vkCreateDevice(m_PhysicalDevice, &deviceInfo, nullptr, &m_Device);

    vkGetDeviceQueue(m_Device, m_GraphicsIndex, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, m_PresentIndex, 0, &m_PresentQueue);
}
VkDevice $::Device::get() { return m_Device; }
VkPhysicalDevice $::Device::getPhysicalDevice() { return m_PhysicalDevice; }
u32 $::Device::getGraphicsIndex() { return m_GraphicsIndex; }
VkResult $::Device::SubmitQueue(VkSubmitInfo& submitInfo, VkFence fence) {
    return vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, fence);
}
VkResult $::Device::PresentQueue(VkPresentInfoKHR& presentInfo) {
    return vkQueuePresentKHR(m_GraphicsQueue, &presentInfo);
}
VkResult $::Device::waitForFences(VkFence* fences, u32 count) {
    return vkWaitForFences(m_Device, count, fences, VK_TRUE, UINT64_MAX);
}
VkResult $::Device::resetFences(VkFence* fences, u32 count) {
    return vkResetFences(m_Device, count, fences);
}



VkSurfaceFormatKHR getSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    u32 count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
    vec<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, formats.data());
    for (auto& i : formats) {
        if (i.format == VK_FORMAT_B8G8R8A8_SRGB && i.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return i;
        }
    }
    return formats[0];
}
VkPresentModeKHR getPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    u32 count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
    vec<VkPresentModeKHR> presentModes(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, presentModes.data());
    for (auto& i : presentModes) {
        if (i == VK_PRESENT_MODE_MAILBOX_KHR) return i;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}
u32 getMinimumImageCount(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    u32 imageCount = 0;
    VkSurfaceCapabilitiesKHR capa;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capa);
    imageCount = capa.minImageCount + 1;
    if (capa.maxImageCount > 0 && imageCount > capa.maxImageCount) {
        imageCount = capa.maxImageCount;
    }
    return imageCount;
}
$::ImageArray CreateSwapchainImagesAndViews(VkDevice device, VkSwapchainKHR swapchain, VkFormat format) {
    $::ImageArray Images;
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
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
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
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
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
$::Swapchain createSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow* window, u32 graphicsIndex, u32 presentIndex) {

    VkSwapchainKHR swapChain;
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

    SurfaceDisplayFormats formats = {surfaceFormat, presentMode, extent};

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    u32 queueFamilyIndices[] = {graphicsIndex, presentIndex};
    VkSwapchainCreateInfoKHR createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;



    if (graphicsIndex != presentIndex) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
    auto images = CreateSwapchainImagesAndViews(device, swapChain, formats.Format.format);

    return $::Swapchain(formats, swapChain, images);
}
$::Swapchain::Swapchain($::Device& device, $::Window& window) {
    m_Formats.Extent = {window.size().x, window.size().y};
    m_Formats.Format = getSurfaceFormat(device.getPhysicalDevice(), window.getSurface());
    m_Formats.PresentMode = getPresentMode(device.getPhysicalDevice(), window.getSurface());
    auto capa = Info::GetSurfaceCapabilities(device.getPhysicalDevice(), window.getSurface());
    u32 minImageCount = getMinimumImageCount(device.getPhysicalDevice(), window.getSurface());
    vec<u32> queueIndices = { device.getGraphicsIndex() };

    auto createInfo = Info::SwapchainCreateInfo(m_Formats, window.getSurface(), minImageCount, capa.currentTransform, queueIndices);

    vkCreateSwapchainKHR(device.get(), &createInfo, nullptr, &m_Swapchain);
    m_Images = CreateSwapchainImagesAndViews(device.get(), m_Swapchain, m_Formats.Format.format);
}
$::Swapchain::Swapchain(SurfaceDisplayFormats formats, VkSwapchainKHR swapchain, $::ImageArray Images) : m_Formats(formats), m_Swapchain(swapchain), m_Images(Images) {};
VkSwapchainKHR $::Swapchain::get() { return m_Swapchain; }
VkExtent2D $::Swapchain::getExtent() { return m_Formats.Extent; }
SurfaceDisplayFormats $::Swapchain::getFormats() { return m_Formats; }
u32 $::Swapchain::getImageCount() { return m_Images.Image.size(); }
VkFramebuffer* $::Swapchain::getFramebuffers() { return m_Framebuffers.data(); };
void $::Swapchain::initializeFramebuffers($::Device& device, VkRenderPass& renderPass) {
    m_Framebuffers.resize(m_Images.Image.size());
    for (int i = 0; i < m_Framebuffers.size(); i++) {
        std::vector<VkImageView> attachments = {m_Images.View[i]};
        auto info = Info::FramebufferCreateInfo(renderPass, attachments, m_Formats.Extent, 1);
        vkCreateFramebuffer(device.get(), &info, nullptr, &m_Framebuffers[i]);
    }
}


$::CommandBuffer::CommandBuffer($::Device &device, VkCommandPool* owner, uint32_t count) : m_OwnerPool(owner), m_Index(0) {
    m_Buffers.resize(count);
    auto info = Info::CommandBufferAllocationInfo(*m_OwnerPool, count, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    auto res = vkAllocateCommandBuffers(device.get(), &info, m_Buffers.data());
}
void $::CommandBuffer::nextIndex() { if (m_Index < m_Buffers.size()-1) m_Index++; }
void $::CommandBuffer::prevIndex() { if (m_Index != 0) m_Index--; }
void $::CommandBuffer::setIndex(u32 index) { m_Index = index; }
void $::CommandBuffer::record(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent) {
    RecordCommandBuffer(m_Buffers[m_Index]);
    BeginRenderPass(extent, renderPass, framebuffer, m_Buffers[m_Index]);
}
void $::CommandBuffer::stopRecording() {
    EndRenderPass(m_Buffers[m_Index]);
    EndCommandBuffer(m_Buffers[m_Index]);
}
void $::CommandBuffer::bindPipeline(VkPipeline pipeline, VkPipelineBindPoint bindPoint) {
    vkCmdBindPipeline(m_Buffers[m_Index], bindPoint, pipeline);
}
void $::CommandBuffer::draw(u32 vertexCount, u32 instances, u32 firstVertex, u32 firstInstance) {
    vkCmdDraw(m_Buffers[m_Index], vertexCount, instances, firstVertex, firstInstance);
}
VkCommandBuffer $::CommandBuffer::get(uint32_t index) {
    return m_Buffers[index];
}




$::CommandPool::CommandPool($::Device device) {
    auto createInfo = Info::CommandPoolCreateInfo(device.getGraphicsIndex());
    vkCreateCommandPool(device.get(), &createInfo, nullptr, &m_Pool);
}
$::CommandBuffer $::CommandPool::AllocateBuffers($::Device& device, u32 count) {
    return CommandBuffer(device, &m_Pool, count);
}


$::RenderPass::RenderPass($::Device& device, $::Swapchain& swapchain) {

    std::vector<VkAttachmentDescription> colorAttchment = {Info::RenderPassAttachmentCreateInfo(swapchain.getFormats().Format.format, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)};
    auto colorAttachmentRef = Info::RenderPassAttachmentReferenceCreateInfo(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    std::vector<VkSubpassDescription> subpass = {Info::SubpassDescriptionCreateInfo(VK_PIPELINE_BIND_POINT_GRAPHICS, colorAttachmentRef)};
    std::vector<VkSubpassDependency> dependancy = {Info::SubpassDependencyCreateInfo(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)};
    auto info = Info::RenderPassCreateInfo(colorAttchment, subpass, dependancy);

    VkResult renderpassResult = vkCreateRenderPass(device.get(), &info, nullptr, &m_RenderPass);
}
VkRenderPass $::RenderPass::get() { return m_RenderPass; }


$::GraphicsPipeline::GraphicsPipeline($::Device &device, $::Swapchain& swapchain, $::RenderPass& renderPass, Shader &vShader, Shader &fShader) {
    auto vertexInfo = Info::Pipeline::ShaderStageCreateInfo(vShader);
    auto fragmentInfo = Info::Pipeline::ShaderStageCreateInfo(fShader);
    VkPipelineShaderStageCreateInfo shaderStages[2] = {vertexInfo,fragmentInfo};

    auto vertexInput = Info::Pipeline::VertexInputCreateInfo({},{});
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
VkPipeline $::GraphicsPipeline::get() { return m_Pipeline; }
