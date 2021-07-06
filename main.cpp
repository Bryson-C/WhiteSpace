#include "lib/VKInclude/vulkan/vulkan.h"
#include "lib/GLFW/include/glfw3.h"
#include "lib/glm-master/glm/glm.hpp"
#include <iostream>
#include <vector>
#include <tuple>
#include <optional>
#include <array>
#include <algorithm>
#include <fstream>



std::string thisDir() {
    std::string file_path = __FILE__;
    std::string dir_path = file_path.substr(0, file_path.rfind("\\"));
    return dir_path;
}
std::string dir(std::string folder) {
    std::string purepath{};
    if (folder.find(thisDir()+'\\') < folder.size()) {
        purepath = folder;
    } else {
        purepath = thisDir() + '\\' + folder;
    }
    return purepath;
}


bool frameBufferResized = false;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        std::cerr << "(" << messageSeverity << ")validation layer: " << pCallbackData->pMessage << "\n" << std::endl;

    return VK_FALSE;
}
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}
static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto pointer = glfwGetWindowUserPointer(window);
    frameBufferResized = true;
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




namespace WS {
    void logError(VkResult result, const char* Object) {
	printf(Object);
	switch (result)
	{
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

     void destroyObjects(VkInstance instances) {
        vkDestroyInstance(instances, nullptr);
    }
    void destroyObjects(VkDevice device) {
        vkDestroyDevice(device, nullptr);
    }
    void destroyObjects(std::tuple<GLFWwindow*, VkSurfaceKHR> SurfaceObjects, VkInstance instance) {
        vkDestroySurfaceKHR(instance, std::get<1>(SurfaceObjects), nullptr);
        glfwDestroyWindow(std::get<0>(SurfaceObjects));
    } 
    void destroyObjects(VkSwapchainKHR swapchain,std::vector<VkImageView> imageViews, VkDevice device) {
        for (auto i : imageViews) {
            vkDestroyImageView(device, i, nullptr);
        }
        vkDestroySwapchainKHR(device, swapchain, nullptr);
    }
    void destroyObjects(VkPipeline pipeline, VkDevice device) {
        vkDestroyPipeline(device, pipeline, nullptr);
    }
    void destroyObjects(VkRenderPass renderPass, VkDevice device) {
        vkDestroyRenderPass(device, renderPass, nullptr);
    }
    void destroyObjects(std::vector<VkFramebuffer> frameBuffers, VkCommandPool commandPool, std::vector<VkCommandBuffer> commandBuffers, VkDevice device) {
        for (auto i : frameBuffers) {
            vkDestroyFramebuffer(device, i, nullptr);
        }
        vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    }
    void destroyObjects(VkCommandPool commandPool, VkDevice device) {
        vkDestroyCommandPool(device, commandPool, nullptr);
    }
    void destroyObjects(std::vector<VkSemaphore> semaphores, VkDevice device) {
        for (auto i : semaphores) {
            vkDestroySemaphore(device, i, nullptr);
        }
    }
    void destroyObjects(std::vector<VkFence> fences, VkDevice device) {
        for (auto i : fences) {
            vkWaitForFences(device, 1, &i, VK_TRUE, UINT64_MAX);
            vkDestroyFence(device, i, nullptr);
        }
    }
    void destroyObjects(VkBuffer buffer, VkDeviceMemory deviceMemory, VkDevice device) {
        vkDestroyBuffer(device, buffer, nullptr);
        vkFreeMemory(device, deviceMemory, nullptr);
    }

    std::tuple<GLFWwindow*, VkSurfaceKHR> createWindow(const char* title,int w, int h, VkInstance instance) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        auto window = glfwCreateWindow(w,h,title, nullptr, nullptr);

        VkSurfaceKHR surface;
        VkResult surfaceResult = glfwCreateWindowSurface(instance, window, nullptr, &surface);
        if (window == nullptr) {
            throw std::runtime_error("Window Failed Creation");
        }
        if (surfaceResult != VK_SUCCESS) {
            logError(surfaceResult, "Window Surface: ");
            throw std::runtime_error("Surface Failed Creation");
        }
        glfwSetWindowUserPointer(window, window);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        return {window, surface};
    }

    VkInstance createInstance(bool debug = false) {
        VkInstance instance;
        
        glfwInit();
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> Extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        std::vector<const char*> ValidationLayers{ "VK_LAYER_KHRONOS_validation" };
        Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);



        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;


        createInfo.enabledExtensionCount = static_cast<uint32_t>(Extensions.size());
        createInfo.ppEnabledExtensionNames = Extensions.data();

        VkDebugUtilsMessengerEXT debugMessenger{};
        VkDebugUtilsMessengerCreateInfoEXT DebugInfo{};
        if (debug) {    
            DebugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            DebugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            DebugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            DebugInfo.pfnUserCallback = debugCallback;
            DebugInfo.pUserData = nullptr; // Optional
            populateDebugMessengerCreateInfo(DebugInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &DebugInfo;
            createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
            createInfo.ppEnabledLayerNames = ValidationLayers.data();
        }

        VkResult instanceResult = vkCreateInstance(&createInfo, nullptr, &instance);
        VkResult layerResult = CreateDebugUtilsMessengerEXT(instance, &DebugInfo, nullptr, &debugMessenger);
        if (instanceResult != VK_SUCCESS) {
            logError(instanceResult, "Instance: ");
            throw std::runtime_error("Instance Failed Creation");
        }
        if (debug) {
            if (layerResult != VK_SUCCESS) {
                logError(layerResult,"Validation Layers: ");
                throw std::runtime_error("Validation Layers Failed Creation");
            }
        }
        return instance;
    }
    VkPhysicalDevice findPhysicalDevice(VkInstance instance) {
        VkPhysicalDevice physicalDevice;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> deviceList(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());
        for (auto i : deviceList) {
            physicalDevice = i;
            break;
        }
        return physicalDevice;
}
    
    struct Queue {
        VkQueue Queue;
        uint32_t Index;
        VkQueueFlags Flag;
    };
    std::vector<WS::Queue> findQueues(VkPhysicalDevice device) {
        uint32_t qCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &qCount, nullptr);
        std::vector<VkQueueFamilyProperties> qFamilyProperties(qCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &qCount, qFamilyProperties.data());

        int loop = 0;
        std::vector<WS::Queue> Queues{};
        for (auto i : qFamilyProperties) {
            WS::Queue q{};
            q.Index = loop;
            q.Flag = i.queueFlags;
            Queues.push_back(q);
            loop++;
        }
        return Queues;
    }
    std::vector<VkDeviceQueueCreateInfo> createQueueInfo(std::vector<WS::Queue> qList) {
        std::vector<VkDeviceQueueCreateInfo> qCreateInfos{};
        for (auto i : qList) {
            float priority = 1.0f;
            VkDeviceQueueCreateInfo qCreateInfo{};
            qCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            qCreateInfo.pQueuePriorities = &priority;
            qCreateInfo.queueCount = 1;
            qCreateInfo.queueFamilyIndex = i.Index;
            qCreateInfos.push_back(qCreateInfo);
        }
        return qCreateInfos;
    }

    VkDevice createDevice(VkPhysicalDevice physicalDevice, std::vector<WS::Queue> queueList) {
        VkDevice device;
        std::vector<const char*> Extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        
        auto queueCreateInfo = createQueueInfo(queueList);


        //VkPhysicalDeviceFeatures deviceFeatures;
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pEnabledFeatures = nullptr;//&deviceFeatures;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfo.size());
        createInfo.pQueueCreateInfos = queueCreateInfo.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(Extensions.size());
        createInfo.ppEnabledExtensionNames = Extensions.data();
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

        VkResult deviceResult = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
        if (deviceResult != VK_SUCCESS) {
            logError(deviceResult, "Device: ");
            throw std::runtime_error("Device Failed Creation");
        }
        return device;
    }
    std::tuple<VkSurfaceFormatKHR, VkPresentModeKHR, VkExtent2D> getPresentationInformation(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow* window) {
        
        uint32_t formatCount = 0;
        uint32_t presentModeCount = 0;




        std::optional<VkSurfaceFormatKHR> SurfaceFormat;
        std::optional<VkPresentModeKHR> PresentMode;
        std::optional<VkExtent2D> Extent;




        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> SurfaceFormats(formatCount);
        if (SurfaceFormats.size() > 0) {
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, SurfaceFormats.data());
            for (auto i : SurfaceFormats) {
                if (i.format == VK_FORMAT_B8G8R8A8_SRGB && i.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    SurfaceFormat = i;
                    break;
                }
            }
            if (!SurfaceFormat.has_value()) {
                SurfaceFormat = SurfaceFormats[0];
            }
        }

        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> PresentModes(presentModeCount);
        if (PresentModes.size() > 0) {
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, PresentModes.data());
            for (auto i : PresentModes) {
                if (i == VK_PRESENT_MODE_MAILBOX_KHR) {
                    PresentMode = i;
                    break;
                }
            }
            if (!PresentMode.has_value()) {
                PresentMode = PresentModes[0];
            }

        }

        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);


        if (capabilities.currentExtent.width != UINT32_MAX) {
            Extent = capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
            Extent = actualExtent;
        }


        return {SurfaceFormat.value(), PresentMode.value(), Extent.value()};
    }
    VkSwapchainKHR createSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, GLFWwindow* window, std::vector<WS::Queue> queueList, VkFormat& imageFormat, VkExtent2D& extent, uint32_t& gQueue, uint32_t& pQueue) {
        VkSwapchainKHR swapchain{};
        
        
        auto SurfaceInfo = getPresentationInformation(physicalDevice, surface, window);

        


        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
        uint32_t imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
            imageCount = capabilities.maxImageCount;
        }

        bool multiQueue = false;
        uint32_t graphicsQueue;
        uint32_t presentQueue = -1;
        

        uint32_t loop = 0;
        for (auto i : queueList) {
            if (i.Flag & VK_QUEUE_GRAPHICS_BIT) {
                graphicsQueue = i.Index;
            }
            if (i.Flag & VK_QUEUE_TRANSFER_BIT && i.Index != graphicsQueue) {
                presentQueue = i.Index;
                multiQueue = true;
            }
        }
        if (presentQueue == -1) {
            for (auto i : queueList) {
                if (i.Flag & VK_QUEUE_TRANSFER_BIT) {
                    presentQueue = i.Index;
                    multiQueue = false;
                }
            }
        }
        gQueue = graphicsQueue;
        pQueue = presentQueue;

        VkBool32 surfaceSupport;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, presentQueue, surface, &surfaceSupport);
        if (!surfaceSupport)
            throw std::runtime_error("This Device Does Not Support Rendering");


        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = std::get<0>(SurfaceInfo).format;
        createInfo.imageColorSpace = std::get<0>(SurfaceInfo).colorSpace;
            imageFormat = std::get<0>(SurfaceInfo).format;
        createInfo.imageExtent = std::get<2>(SurfaceInfo);
            extent = std::get<2>(SurfaceInfo);
        createInfo.imageArrayLayers  = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (multiQueue) {
            uint32_t queueIndices[] {graphicsQueue, presentQueue};
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueIndices;
        } else if (!multiQueue || graphicsQueue == presentQueue) {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }
        createInfo.preTransform = capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = std::get<1>(SurfaceInfo);
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_FALSE;



        VkResult swapchainResult = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain);
        if (swapchainResult != VK_SUCCESS) {
            logError(swapchainResult, "Swapchain: ");
            throw std::runtime_error("Swaphchain Failed Creation");
        }

        return swapchain;
    }
    std::vector<VkImage> getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain) {
        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
        std::vector<VkImage> images(imageCount);
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());
        return images;
    }
    std::vector<VkImageView> createImageViews(std::vector<VkImage> images, VkDevice device, VkFormat imageFormat) {
        std::vector<VkImageView> imageViews{}; imageViews.resize(images.size());
        int loop = 0;
        for (auto i : imageViews) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = images[loop];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = imageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            if (vkCreateImageView(device, &createInfo, nullptr, &imageViews[loop]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
            loop++;
        }
        return imageViews;
    }
    
    VkShaderModule createShaderModule(const char* filename, VkDevice device) {
    
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Could Not Read Shaders");
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        //return buffer;


        VkShaderModuleCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        CreateInfo.codeSize = fileSize;
        CreateInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());


        VkShaderModule Shader{};
        VkResult result = vkCreateShaderModule(device, &CreateInfo, nullptr, &Shader);
        if (result != VK_SUCCESS) {
            WS::logError(result, "Shader Module: ");
            throw std::runtime_error("Failed To Create Shader Module");
        }
        return Shader;       
    }
    VkRenderPass createRenderpass(VkDevice device, VkFormat imageFormat) {
        VkRenderPass renderPass;

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = imageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependancy{};
        dependancy.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependancy.dstSubpass = 0;
        dependancy.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependancy.srcAccessMask = 0;
        dependancy.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependancy.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependancy;


        VkResult renderpassResult = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
        if (renderpassResult != VK_SUCCESS) {
            logError(renderpassResult, "Render Pass: ");
            throw std::runtime_error("Render Pass Failed Creation");
        }

        return renderPass;
    }
    VkPipeline createGraphicsPipeline(VkDevice device,VkShaderModule vertexShader, VkShaderModule fragmentShader, VkRenderPass renderPass, VkExtent2D swapchainExtent,VkPhysicalDevice physicalDevice, VkPolygonMode DrawMode = VK_POLYGON_MODE_FILL) {
        VkPipeline graphicsPipeline{};


        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertexShader;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragmentShader;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] {vertShaderStageInfo, fragShaderStageInfo};


        auto bindingDescription = vertex::getBindingDescription();
        auto attributeDescriptions = vertex::getAttributeDescriptions();


        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();


        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;


        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapchainExtent.width;
        viewport.height = (float) swapchainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;


        VkRect2D scissor{};
        scissor.offset = {0,0};
        scissor.extent = swapchainExtent;


        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;


        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;

        VkPhysicalDeviceFeatures Features;
        vkGetPhysicalDeviceFeatures(physicalDevice, &Features);
        if (!Features.fillModeNonSolid) {
            DrawMode = VK_POLYGON_MODE_FILL;
        }
        rasterizer.polygonMode = DrawMode;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional


        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional


        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;


        VkPipelineLayout pipelineLayout{};
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
        
        
        VkResult pipelineResult = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
        if (pipelineResult != VK_SUCCESS) {
            logError(pipelineResult,"Pipeline Layout: ");
            throw std::runtime_error("Pipeline Layout Failed Creation");
        }


        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr; // Optional;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional




        VkResult graphicsResult = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline); // fails here
        
        if (graphicsResult != VK_SUCCESS) {
            logError(graphicsResult,"Graphics Pipeline: ");
            throw std::runtime_error("Graphics Pipeline Failed Creation");
        }
        return graphicsPipeline;
    }
    std::vector<VkFramebuffer> createFramebuffers(std::vector<VkImageView> imageViews,VkRenderPass renderPass,VkExtent2D extent, VkDevice device) {
        std::vector<VkFramebuffer> frameBuffers{}; frameBuffers.resize(imageViews.size());

        int loop = 0;
        for (auto i : frameBuffers) {
            VkImageView attachment[] {
                imageViews[loop]
            };

            VkFramebufferCreateInfo frameBufferInfo{};
            frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            frameBufferInfo.renderPass = renderPass;
            frameBufferInfo.attachmentCount = 1;
            frameBufferInfo.pAttachments = attachment;
            frameBufferInfo.width = extent.width;
            frameBufferInfo.height = extent.height;
            frameBufferInfo.layers = 1;

            VkResult fbResult = vkCreateFramebuffer(device, &frameBufferInfo, nullptr, &frameBuffers[loop]);
            if (fbResult != VK_SUCCESS) {
                logError(fbResult, "Framebuffers: ");
                throw std::runtime_error("Framebuffer Failed Creation");
            }

            loop++;
        }
        return frameBuffers;
    }
    VkCommandPool createCommandPool(VkDevice device, uint32_t graphicsQueue) {
        VkCommandPool commandPool;

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = graphicsQueue;
        poolInfo.flags = 0; // Optional
        VkResult commandPoolResult = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
        if (commandPoolResult != VK_SUCCESS) {
            logError(commandPoolResult, "Command Pool: ");
            throw std::runtime_error("failed to create command pool!");
        }
        return commandPool;
    }
    std::vector<VkCommandBuffer> createCommandBuffers(VkDevice device, VkCommandPool commandPool, std::vector<VkFramebuffer> framebuffers, VkRenderPass renderPass, VkPipeline graphicsPipeline, VkExtent2D extent, VkBuffer vertexBuffer = VK_NULL_HANDLE) {
        std::vector<VkCommandBuffer> commandBuffers{}; commandBuffers.resize(framebuffers.size());
        
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (size_t i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = framebuffers[i];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = extent;


            VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;


            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

            if (vertexBuffer != VK_NULL_HANDLE) {
                VkBuffer vertexBuffers[] {vertexBuffer};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
            }

            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }

        }
        return commandBuffers;
    }
    std::vector<VkSemaphore> createSemaphores(const size_t semaphoreCount, VkDevice device) {
        std::vector<VkSemaphore> semaphores{};
        for (int i = 0; i < semaphoreCount; i++) {
            VkSemaphore sema{};
            VkSemaphoreCreateInfo semaInfo{};
            semaInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkResult SemaResult = vkCreateSemaphore(device, &semaInfo, nullptr, &sema);
            if (SemaResult != VK_SUCCESS) {
                logError(SemaResult, "Semaphore: ");
                throw std::runtime_error("Semaphore Failed Creation");
            }
            semaphores.push_back(sema);
        }
        return semaphores;
    }
    std::vector<VkFence> createFences(const size_t fenceCount, VkDevice device) {
        std::vector<VkFence> fences;
        for (int i = 0; i < fenceCount; i++) {
            VkFence fence;
            VkFenceCreateInfo fInfo{};
            fInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            VkResult fResult = vkCreateFence(device, &fInfo, nullptr, &fence);
            if (fResult != VK_SUCCESS) {
                logError(fResult, "Fence: ");
                throw std::runtime_error("Fence Failed Creation");
            }
            fences.push_back(fence);
        }
        return fences;
    }
    void cleanSwapchain(VkDevice device, std::vector<VkFramebuffer> framebuffers, VkCommandPool commandPool, std::vector<VkCommandBuffer> commandBuffers, VkPipeline graphicsPipeline, VkRenderPass renderPass, VkSwapchainKHR swapchain, std::vector<VkImageView> imageViews) {
        WS::destroyObjects(framebuffers, commandPool, commandBuffers, device);
        WS::destroyObjects(graphicsPipeline, device);
        WS::destroyObjects(renderPass, device);
        WS::destroyObjects(swapchain, imageViews, device);
    }
    void recreateSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, const char* vertexPath, const char* fragmentPath, std::tuple<GLFWwindow*, VkSurfaceKHR> SurfaceObjects, std::vector<WS::Queue> queueList,
    std::vector<VkFramebuffer>& framebuffers, VkCommandPool commandPool, std::vector<VkCommandBuffer>& commandBuffers, VkPipeline& graphicsPipeline, VkRenderPass& renderPass, VkSwapchainKHR& swapchain, std::vector<VkImageView>& imageViews, VkPolygonMode DrawMode = VK_POLYGON_MODE_FILL) {
        int width = 0, height = 0;
        glfwGetFramebufferSize(std::get<0>(SurfaceObjects),&width,&height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(std::get<0>(SurfaceObjects), &width, &height);
            glfwWaitEvents();
        }



        vkDeviceWaitIdle(device);

        cleanSwapchain(device,framebuffers, commandPool, commandBuffers, graphicsPipeline, renderPass, swapchain, imageViews);

        VkFormat imageFormat;
        VkExtent2D extent;
        uint32_t graphicsIndex;
        uint32_t presentIndex;


        auto window = std::get<0>(SurfaceObjects);
        auto surface = std::get<1>(SurfaceObjects);



        swapchain = createSwapchain(physicalDevice, device, surface, window, queueList, imageFormat, extent, graphicsIndex, presentIndex);
        auto SwapchainImages = getSwapchainImages(device, swapchain);
        imageViews = createImageViews(SwapchainImages, device, imageFormat);
        renderPass = createRenderpass(device, imageFormat);
        


        auto vertexShader = createShaderModule(vertexPath,device);
        auto fragmentShader = createShaderModule(fragmentPath,device);
        graphicsPipeline = createGraphicsPipeline(device, vertexShader, fragmentShader, renderPass, extent, physicalDevice, DrawMode);
        framebuffers = createFramebuffers(imageViews, renderPass, extent, device);
        commandBuffers = createCommandBuffers(device, commandPool, framebuffers, renderPass, graphicsPipeline, extent);


    }
    VkBuffer createVertexBuffer(std::vector<vertex> vertices, VkDevice device, VkDeviceSize& size) {
        VkBuffer Buffer;
        VkBufferCreateInfo BufferInfo{};
        BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferInfo.size = sizeof(vertices[0]) * vertices.size();
        size = BufferInfo.size;
        BufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult bufferResult = vkCreateBuffer(device, &BufferInfo, nullptr, &Buffer);
        if (bufferResult != VK_SUCCESS) {
            logError(bufferResult, "Vertex Buffer: ");
            throw std::runtime_error("Buffer Failed Creation");
        }
        return Buffer;
    }
    uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        throw std::runtime_error("Failed To Find Memory Type");
    }
    void allocateVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer buffer, VkDeviceMemory deviceMemory, VkDeviceSize size, std::vector<vertex> vertices) {
        VkMemoryRequirements memRequirements{};
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        
        VkResult allocResult = vkAllocateMemory(device, &allocInfo, nullptr, &deviceMemory);
        if (allocResult != VK_SUCCESS) {
            logError(allocResult, "Buffer Allocation: ");
            throw std::runtime_error("Failed Buffer Allocation");
        }
        vkBindBufferMemory(device, buffer, deviceMemory, 0);

        void* data;
        vkMapMemory(device, deviceMemory, 0, size, 0, &data);
        memcpy(data, vertices.data(), size_t(size));
        vkUnmapMemory(device, deviceMemory);
    }
}



int main() {

    

    auto Instance = WS::createInstance(true);
    auto PhysicalDevice = WS::findPhysicalDevice(Instance);
    auto queueList = WS::findQueues(PhysicalDevice);
    auto Device = WS::createDevice(PhysicalDevice, queueList);
    auto WindowTitle = "Window";
    auto SurfaceObjects =  WS::createWindow(WindowTitle, 800, 600, Instance);
    auto Window = std::get<0>(SurfaceObjects);
    auto Surface = std::get<1>(SurfaceObjects);
    VkFormat imageFormat;
    VkExtent2D imageExtent;
    uint32_t graphicsQueueIndex;
    uint32_t presentQueueIndex;
    VkQueue GraphicsQueue;
    VkQueue PresentQueue;
    
    auto Swapchain = WS::createSwapchain(PhysicalDevice, Device, Surface, Window, queueList, imageFormat, imageExtent, graphicsQueueIndex, presentQueueIndex);
    auto SwapchainImages = WS::getSwapchainImages(Device, Swapchain);
    auto SwapchainImageViews = WS::createImageViews(SwapchainImages, Device, imageFormat);

    vkGetDeviceQueue(Device, graphicsQueueIndex, 0, &GraphicsQueue);
    vkGetDeviceQueue(Device, presentQueueIndex, 0, &PresentQueue);


    auto VertexShader = WS::createShaderModule(dir("shaders\\defaultVertex.spv").c_str(), Device);
    auto FragmentShader = WS::createShaderModule(dir("shaders\\defaultFragment.spv").c_str(), Device);
    auto RenderPass = WS::createRenderpass(Device, imageFormat);
    auto DrawMode = VK_POLYGON_MODE_LINE;
    auto GraphicsPipeline = WS::createGraphicsPipeline(Device, VertexShader, FragmentShader, RenderPass, imageExtent, PhysicalDevice, DrawMode);
    auto FrameBuffers = WS::createFramebuffers(SwapchainImageViews, RenderPass, imageExtent, Device);
    auto CommandPool = WS::createCommandPool(Device, graphicsQueueIndex);
    auto CommandBuffers = WS::createCommandBuffers(Device, CommandPool, FrameBuffers, RenderPass, GraphicsPipeline, imageExtent);

    const int MAX_FRAMES_IN_FLIGHT = 2;
    auto ImageSemaphores = WS::createSemaphores(MAX_FRAMES_IN_FLIGHT, Device);
    auto RenderSemaphores = WS::createSemaphores(MAX_FRAMES_IN_FLIGHT, Device);
    auto Fences = WS::createFences(MAX_FRAMES_IN_FLIGHT, Device);
    std::vector<VkFence> Images{}; Images.resize(SwapchainImages.size(), VK_NULL_HANDLE);
    
    const std::vector<vertex> vertices {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };
    VkDeviceSize VertexBufferSize;
    auto VertexBuffer = WS::createVertexBuffer(vertices,Device, VertexBufferSize);
    VkDeviceMemory VertexBufferMemory{};
    WS::allocateVertexBuffer(Device, PhysicalDevice, VertexBuffer, VertexBufferMemory, VertexBufferSize, vertices);



    int currentFrame = 0;
    frameBufferResized = false;
    int globalClock = clock();
    while(!glfwWindowShouldClose(Window)) { 
        if (clock()-globalClock < 3000) {
            glfwSetWindowTitle(Window, "[Success]");
        } else {
            glfwSetWindowTitle(Window, WindowTitle);
        }



        vkWaitForFences(Device, 1, &Fences[currentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(Device, 1, &Fences[currentFrame]);



        uint32_t imageIndex = 0;
        VkResult imageResult = vkAcquireNextImageKHR(Device, Swapchain, UINT64_MAX, ImageSemaphores[currentFrame], nullptr, &imageIndex);

        if (imageResult == VK_ERROR_OUT_OF_DATE_KHR) {
            WS::recreateSwapchain(Device, PhysicalDevice, dir("shaders\\defaultVertex.spv").c_str(), dir("shaders\\defaultFragment.spv").c_str(), SurfaceObjects, 
            queueList, FrameBuffers, CommandPool, CommandBuffers, GraphicsPipeline, RenderPass, Swapchain, SwapchainImageViews, DrawMode);
        }
        else if (imageResult != VK_SUCCESS && imageResult == VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed To Get Swapchain Image");
        }

       
        // I get errors from code below
        //if (Images[imageIndex] != VK_NULL_HANDLE) {
        //    vkWaitForFences(Device, 1, &Images[imageIndex], VK_TRUE, UINT64_MAX);
        //}
        //Images[imageIndex] = Fences[currentFrame];

    


        VkSubmitInfo submitinfo{};
        VkSemaphore waitSemaphores[] = { ImageSemaphores[currentFrame] };
        VkSemaphore signalSemaphore[] = { RenderSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSwapchainKHR swapchains[] = { Swapchain };


        submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitinfo.waitSemaphoreCount = 1;
        submitinfo.pWaitSemaphores = waitSemaphores;
        submitinfo.pWaitDstStageMask = waitStages;
        submitinfo.commandBufferCount = 1;
        submitinfo.pCommandBuffers = &CommandBuffers[imageIndex];
        submitinfo.signalSemaphoreCount = 1;
        submitinfo.pSignalSemaphores = signalSemaphore;



        VkResult SubmitResult = vkQueueSubmit(GraphicsQueue, 1, &submitinfo, Fences[currentFrame]);
        if (SubmitResult != VK_SUCCESS) {
            WS::logError(SubmitResult, "Queue Submition: ");
            throw std::runtime_error("Failed To Submit Queue");
        }


        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        VkResult presentResult = vkQueuePresentKHR(PresentQueue, &presentInfo);



        
        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || frameBufferResized) {
            frameBufferResized = false;
            WS::recreateSwapchain(Device, PhysicalDevice, dir("shaders\\defaultVertex.spv").c_str(), dir("shaders\\defaultFragment.spv").c_str(), SurfaceObjects, 
            queueList, FrameBuffers, CommandPool, CommandBuffers, GraphicsPipeline, RenderPass, Swapchain, SwapchainImageViews, DrawMode);
        }
        else if (presentResult != VK_SUCCESS ) {
            throw std::runtime_error("Failed To Get Swapchain Image");
        }

        

        glfwPollEvents();

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }


    WS::destroyObjects(Fences, Device);
    WS::destroyObjects(Images, Device);
    WS::destroyObjects(RenderSemaphores, Device);
    WS::destroyObjects(ImageSemaphores, Device);
    WS::destroyObjects(FrameBuffers, CommandPool, CommandBuffers, Device);
    WS::destroyObjects(CommandPool, Device);
    WS::destroyObjects(RenderPass, Device);
    WS::destroyObjects(GraphicsPipeline, Device);
    WS::destroyObjects(Swapchain, SwapchainImageViews, Device);
    WS::destroyObjects(VertexBuffer, VertexBufferMemory, Device);
    WS::destroyObjects(SurfaceObjects, Instance);
    WS::destroyObjects(Device);
    WS::destroyObjects(Instance);


    return 0;
}

/*#define GLFW_INCLUDE_VULKAN
#include "lib/VKInclude/vulkan/vulkan.h"
#include "lib/GLFW/include/glfw3.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);

        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);
        vkDestroyDevice(device, nullptr);

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    void createSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    }

    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
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

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    void createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void createGraphicsPipeline() {
        auto vertShaderCode = readFile("C:\\Users\\Owner\\Documents\\code\\Vulkan2\\shaders\\defaultVertex.spv");
        auto fragShaderCode = readFile("C:\\Users\\Owner\\Documents\\code\\Vulkan2\\shaders\\defaultFragment.spv");

        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }

    VkShaderModule createShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
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

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
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

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
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

    bool isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    static std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
};


int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}*/