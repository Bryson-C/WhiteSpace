#include "Engine.hpp"



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

void WS::logError(VkResult result, const char* Object) {
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



VkApplicationInfo WSDefaultCreation::ApplicationInfo() { return {VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, "Vulkan Instance", VK_MAKE_VERSION(1,0,0), "WhiteSpace", VK_MAKE_VERSION(0,1,0),VK_API_VERSION_1_2}; };

    
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


VkDeviceCreateInfo WSDefaultCreation::DeviceInfo(WS::QueueList queueList, WS::Extensions Extensions) { return {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0, static_cast<uint32_t>(queueList.size()), createQueueInfo(queueList).data(),
																			 0/*LayerCount*/, nullptr/*pEnabledLayerNames*/, static_cast<uint32_t>(Extensions.size())/*ExtensionCount*/, Extensions.data()/*pExtensionNames*/, 0 /*Features*/}; } 


WS::SurfaceObjects WS::createWindow(const char* title,int w, int h, VkInstance instance) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        auto window = glfwCreateWindow(w,h,title, nullptr, nullptr);
        VkSurfaceKHR surface;
        VkResult surfaceResult = glfwCreateWindowSurface(instance, window, nullptr, &surface);
        if (window == nullptr) {
            throw std::runtime_error("Window Failed Creation");
        }
        if (surfaceResult != VK_SUCCESS) {
            WS::logError(surfaceResult, "Window Surface: ");
            throw std::runtime_error("Surface Failed Creation");
        }
        return {window, surface};
    }

VkInstance WS::createInstance(bool debug) {
    VkInstance instance;
    VkApplicationInfo Appinfo{};
    VkInstanceCreateInfo CreateInfo{};



        
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
    appInfo.apiVersion = VK_API_VERSION_1_2;

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
        WS::logError(instanceResult, "Instance: ");
        throw std::runtime_error("Instance Failed Creation");
    }
    if (debug) {
        if (layerResult != VK_SUCCESS) {
            WS::logError(layerResult,"Validation Layers: ");
            throw std::runtime_error("Validation Layers Failed Creation");
        }
    }
    return instance;


}
VkSampleCountFlagBits getMaxMsaaSampleCount(VkPhysicalDevice physicalDevice) {

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

void logSampleCount(VkSampleCountFlagBits flag) {
    switch (flag)
    {
    case VK_SAMPLE_COUNT_64_BIT: printf("Sample Count: 64\n"); break;
    case VK_SAMPLE_COUNT_32_BIT: printf("Sample Count: 32\n"); break;
    case VK_SAMPLE_COUNT_16_BIT: printf("Sample Count: 16\n"); break;
    case VK_SAMPLE_COUNT_8_BIT: printf("Sample Count: 8\n"); break;
    case VK_SAMPLE_COUNT_4_BIT: printf("Sample Count: 4\n"); break;
    case VK_SAMPLE_COUNT_2_BIT: printf("Sample Count: 2\n"); break;
    case VK_SAMPLE_COUNT_1_BIT: printf("Sample Count: 1\n"); break;
    default: printf("Sample Count: Unidentified\n"); break;
    }



}

VkPhysicalDevice WS::findPhysicalDevice(VkInstance instance) {
    VkPhysicalDevice SelectedDevice {};
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> gpu(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, gpu.data());
    for (int i = 0; i < gpu.size(); i++) {
        SelectedDevice = gpu[i];
        break;
    }
    auto msaa = getMaxMsaaSampleCount(SelectedDevice);
    printf("PhysicalDevice Sample Count: \n\t");
    logSampleCount(msaa);
    return SelectedDevice;
}

WS::QueueList WS::findQueues(VkPhysicalDevice device) {
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

VkDevice WS::createDevice(VkPhysicalDevice physicalDevice, WS::QueueList& queueList) {
        VkDevice device;
        std::vector<const char*> Extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        
        auto queueCreateInfo = createQueueInfo(queueList);


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
            WS::logError(deviceResult, "Device: ");
            throw std::runtime_error("Device Failed Creation");
        }
        for (auto& i : queueList) {
            vkGetDeviceQueue(device, i.Index, 0, &i.Queue);
        }
        return device;
    }

WS::PresentationInfo getPresentationInformation(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow* window) {
        
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

VkSwapchainKHR WS::createSwapchain(WS::Devices devices, WS::SurfaceObjects surfaceObjects, WS::QueueList queueList, VkFormat& imageFormat, VkExtent2D& extent, uint32_t& gQueue, uint32_t& pQueue) {
        VkSwapchainKHR swapchain{};
        

        VkPhysicalDevice physicalDevice = devices.PhysicalDevice;
        VkDevice device = devices.Device;
        GLFWwindow* window = surfaceObjects.Window;
        VkSurfaceKHR surface = surfaceObjects.Surface;


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
        createInfo.imageFormat = SurfaceInfo.Format.format;
        createInfo.imageColorSpace = SurfaceInfo.Format.colorSpace;
            imageFormat = SurfaceInfo.Format.format;
        createInfo.imageExtent = SurfaceInfo.Extent;
            extent = SurfaceInfo.Extent;
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
        createInfo.presentMode = SurfaceInfo.PresentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_FALSE;



        VkResult swapchainResult = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain);
        if (swapchainResult != VK_SUCCESS) {
            WS::logError(swapchainResult, "Swapchain: ");
            throw std::runtime_error("Swaphchain Failed Creation");
        }

        return swapchain;
    }

WS::ImageList WS::getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain) {
        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
        std::vector<VkImage> images(imageCount);
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());
        return images;
}

WS::ImageViews WS::createImageViews(WS::ImageList images, VkDevice device, VkFormat imageFormat) {

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

VkShaderModule WS::createShaderModule(const char* filename, VkDevice device) { 
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

WS::ShaderProgram WS::createShaderProgram(VkDevice device, const char* vShader, const char* fShader) {
    auto vertex = WS::createShaderModule(vShader, device);
    auto fragment = WS::createShaderModule(fShader, device);
    return {vertex, fragment};
}

VkDescriptorSetLayout WS::createDescriptorSetLayout(VkDevice device) {
    VkDescriptorSetLayoutBinding uboBinding{};
    uboBinding.binding = 0;
    uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboBinding.descriptorCount = 1;
    uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayout descriptorSetLayout{};
    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = 1;
    createInfo.pBindings = &uboBinding;

    VkResult uboResult = vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &descriptorSetLayout);
    if (uboResult != VK_SUCCESS) {
        WS::logError(uboResult,"Descriptor Set Layout: ");
        throw std::runtime_error("Descriptor Set Layout Failed Creation");
    }
    return descriptorSetLayout;
};

VkDescriptorPool WS::createDescriptorPool(VkDevice device, std::vector<VkImage>swapchainImages) {
    VkDescriptorPool pool;
    
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount =  static_cast<uint32_t>(swapchainImages.size());

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(swapchainImages.size());

    VkResult poolResult = vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);
    if (poolResult != VK_SUCCESS) {
        WS::logError(poolResult, "Descriptor Pool: ");
        throw std::runtime_error("Descriptor Pool Failed Creation");
    }
    return pool;
}

std::vector<VkDescriptorSet> WS::createDescriptorSet(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, std::vector<VkImage> swapchainImages, std::vector<VkBuffer> uniformBuffers) {
    std::vector<VkDescriptorSet> descriptorSets{};
    std::vector<VkDescriptorSetLayout> layouts(swapchainImages.size(), descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchainImages.size());
    allocInfo.pSetLayouts = layouts.data();
    VkResult allocResult = vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data());
    if (allocResult != VK_SUCCESS) {
        WS::logError(allocResult, "Descriptor Set Allocation: ");
        throw std::runtime_error("Descriptor Set Failed Allocation");
    }
    for (size_t i = 0; i < swapchainImages.size(); i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(WS::uniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional
        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
    return descriptorSets;
}

VkRenderPass WS::createRenderPass(VkDevice device, VkFormat imageFormat) {
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
        WS::logError(renderpassResult, "Render Pass: ");
        throw std::runtime_error("Render Pass Failed Creation");
    }

    return renderPass;
}




VkPipeline WS::createGraphicsPipeline(WS::Devices devices, WS::ShaderProgram program, VkRenderPass renderPass, VkExtent2D swapchainExtent, VkPipelineLayout& Layout, VkPolygonMode DrawMode, VkDescriptorSetLayout* descriptorSet) {
    VkPipeline graphicsPipeline{};


    auto vertexShader = program[0];
    auto fragmentShader = program[1];
    auto physicalDevice = devices.PhysicalDevice;
    auto device = devices.Device;




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


    auto bindingDescription = WS::vertex::getBindingDescription();
    auto attributeDescriptions = WS::vertex::getAttributeDescriptions();


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
    if (descriptorSet != VK_NULL_HANDLE) {
        pipelineLayoutInfo.setLayoutCount = 1; // Optional
        pipelineLayoutInfo.pSetLayouts = descriptorSet; // Optional
    }

    VkPushConstantRange pushConstRange{};
    pushConstRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstRange.offset = 0;
    pushConstRange.size = sizeof(WS::PushConstantData);

    pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
    pipelineLayoutInfo.pPushConstantRanges = &pushConstRange; // Optional

    
    VkResult pipelineResult = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
    if (pipelineResult != VK_SUCCESS) {
        WS::logError(pipelineResult,"Pipeline Layout: ");
        throw std::runtime_error("Pipeline Layout Failed Creation");
    }
    Layout = pipelineLayout;


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
        WS::logError(graphicsResult,"Graphics Pipeline: ");
        throw std::runtime_error("Graphics Pipeline Failed Creation");
    }
    return graphicsPipeline;
}

uint32_t WS::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("Failed To Find Memory Type");
}

VkBuffer WS::createBuffer(WS::Devices devices, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory, VkDeviceSize size) {
        auto physicalDevice = devices.PhysicalDevice;
        auto device = devices.Device;
        
        VkBuffer Buffer;
        VkBufferCreateInfo BufferInfo{};
        BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferInfo.size = size;
        BufferInfo.usage = usage;
        BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult bufferResult = vkCreateBuffer(device, &BufferInfo, nullptr, &Buffer);
        if (bufferResult != VK_SUCCESS) {
            WS::logError(bufferResult, "Buffer: ");
            throw std::runtime_error("Buffer Failed Creation");
        }


        VkMemoryRequirements memRequirements{};
        VkDeviceMemory deviceMemory{};
        vkGetBufferMemoryRequirements(device, Buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);
        VkResult allocResult = vkAllocateMemory(device, &allocInfo, nullptr, &deviceMemory);
        if (allocResult != VK_SUCCESS) {
            WS::logError(allocResult, "Buffer Allocation: ");
            throw std::runtime_error("Failed Buffer Allocation");
        }
        vkBindBufferMemory(device, Buffer, deviceMemory, 0);
        bufferMemory = deviceMemory;

        return Buffer;
    }


/*
VkDeviceMemory StagingBufferMemory;
auto StagingBuffer = WS::createBuffer(Device, PhysicalDevice, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBufferMemory, sizeof(vertices[0]) * vertices.size());
WS::allocateBuffer(Device, StagingBufferMemory, vertices.data(), sizeof(vertices[0]) * vertices.size());
*/

WS::BufferObjects WS::createStagingBuffer(WS::Devices devices, VkDeviceSize SizeOfData) {
    VkDeviceMemory BufferMemory;
    auto Buffer = WS::createBuffer(devices, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, BufferMemory, SizeOfData);
    return {Buffer, BufferMemory};
}

/*
auto VertexBuffer = WS::createBuffer(Device, PhysicalDevice, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VertexBufferMemory, sizeof(vertices[0]) * vertices.size());
*/

WS::BufferObjects WS::createVertexBuffer(WS::Devices devices, VkDeviceSize SizeOfDevice) {
    VkDeviceMemory BufferMemory;
    auto Buffer = WS::createBuffer(devices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, BufferMemory, SizeOfDevice);
    return { Buffer, BufferMemory };
}

WS::BufferObjects WS::createIndexBuffer(WS::Devices devices, VkDeviceSize sizeOfData, std::vector<uint16_t> indices) {
    VkDeviceMemory BufferMemory;
    if (indices.size() < 6) {
        indices = { 0,1,2,2,3,0 };
    }
    auto Buffer = WS::createBuffer(devices, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, BufferMemory, sizeof(indices[0]) * indices.size());
    return {Buffer, BufferMemory};
}


void WS::allocateBuffer(VkDevice device, VkDeviceMemory& deviceMemory, const void* dataToCopy, VkDeviceSize size) {
    void* data;
    vkMapMemory(device, deviceMemory, 0, size, 0, &data);
    memcpy(data, dataToCopy, size_t(size));
    vkUnmapMemory(device, deviceMemory);
}

void WS::allocateBuffer(VkDevice device, WS::BufferObjects& BufferObjects, const void* dataToCopy, VkDeviceSize size) {
    WS::allocateBuffer(device, BufferObjects.Memory, dataToCopy, size);
}

void WS::copyBuffer(VkDevice device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool, VkQueue queue) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    VkResult allocResult = vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
    if (allocResult != VK_SUCCESS) {
        WS::logError(allocResult, "Command Buffer Allocation: ");
        throw std::runtime_error("Failed To Allocate Command Buffer");
    }
    VkCommandBufferBeginInfo beginfo{};
    beginfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginfo);
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void WS::copyBuffer(VkDevice device, BufferObjects srcBuffer, BufferObjects dstBuffer, VkDeviceSize size, VkCommandPool commandPool, VkQueue queue) {
    WS::copyBuffer(device, srcBuffer.Buffer, dstBuffer.Buffer, size, commandPool, queue);
}

WS::Framebuffers WS::createFrameBuffers(VkDevice device, WS::ImageViews imageViews,VkRenderPass renderPass, VkExtent2D extent) {
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
                WS::logError(fbResult, "Framebuffers: ");
                throw std::runtime_error("Framebuffer Failed Creation");
            }

            loop++;
        }
        return frameBuffers;
    }

VkCommandPool WS::createCommandPool(VkDevice device, uint32_t queue, uint32_t flags) {
    VkCommandPool commandPool;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queue;
    poolInfo.flags = flags;
    VkResult commandPoolResult = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
    if (commandPoolResult != VK_SUCCESS) {
        WS::logError(commandPoolResult, "Command Pool: ");
        throw std::runtime_error("failed to create command pool!");
    }
    return commandPool;
}

WS::CommandBuffers WS::createCommandBuffers(WS::Framebuffers frameBuffers) {
    WS::CommandBuffers commandBuffers; commandBuffers.resize(frameBuffers.size());
    return commandBuffers;
}

void WS::startCmdBufferRecording(VkFramebuffer framebuffer, VkCommandBuffer& commandBuffer, VkCommandPool commandPool, VkDevice device, VkRenderPass& renderPass, VkExtent2D extent) {

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkResult allocResult = vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
    if (allocResult != VK_SUCCESS) {
        WS::logError(allocResult, "Command Buffer Allocation: ");
        throw std::runtime_error("Command Buffer Failed To Allocate");
    }



        VkCommandBufferBeginInfo beginfo{};
        beginfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginfo.pInheritanceInfo = nullptr;
        beginfo.flags = 0;

        if (vkBeginCommandBuffer(commandBuffer, &beginfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed To Begin Recording Of Command Buffers");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = extent;

        VkClearValue clearColor = {0.02f, 0.02f, 0.02f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void WS::stopCmdBufferRecording(VkCommandBuffer commandBuffers) {
    vkCmdEndRenderPass(commandBuffers);
    if (vkEndCommandBuffer(commandBuffers) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}


WS::CommandBuffers WS::createCommandBuffers(VkDevice device, VkCommandPool commandPool, WS::Framebuffers framebuffers, VkRenderPass renderPass, VkPipeline graphicsPipeline, VkExtent2D extent, 
    VkBuffer vertexBuffer, std::vector<uint16_t> indices, WS::BufferObjects indexBuffer, WS::DescriptorSets descriptorSet, VkPipelineLayout* pipelineLayout) {
    





    
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


        VkClearValue clearColor = {0.02f, 0.02f, 0.02f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;


        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        if (vertexBuffer != VK_NULL_HANDLE) {
            VkBuffer vertexBuffers[] {vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
        }

    
        float yOff = -0.2f;


        if (indexBuffer.Buffer != VK_NULL_HANDLE && indices.size() > 0) {
            vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);
            vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1,0,0,0);
        } else {
            if (pipelineLayout != VK_NULL_HANDLE) {
                for (int j = 0; j < 4; j++) {
                    WS::PushConstantData push{};
                    push.offset = {0.0f,yOff * j};
                    push.color = {0.0f,0.0f,0.2f * (j + 1)};
                
                    vkCmdPushConstants(commandBuffers[i], *pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(WS::PushConstantData), &push);
                    vkCmdDraw(commandBuffers[i],3,1,0,0);
                }
            } else {
                vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
            }
        }


        if (descriptorSet.size() > 0 && pipelineLayout != nullptr) {
            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, *pipelineLayout, 0, 1, &descriptorSet[i], 0, nullptr);
        }
        


        vkCmdEndRenderPass(commandBuffers[i]);
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }


    }
    return commandBuffers;
}

WS::SemaphoreList WS::createSemaphores(const size_t semaphoreCount, VkDevice device) {
        std::vector<VkSemaphore> semaphores{};
        for (int i = 0; i < semaphoreCount; i++) {
            VkSemaphore sema{};
            VkSemaphoreCreateInfo semaInfo{};
            semaInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkResult SemaResult = vkCreateSemaphore(device, &semaInfo, nullptr, &sema);
            if (SemaResult != VK_SUCCESS) {
                WS::logError(SemaResult, "Semaphore: ");
                throw std::runtime_error("Semaphore Failed Creation");
            }
            semaphores.push_back(sema);
        }
        return semaphores;
    }

WS::FenceList WS::createFences(const size_t fenceCount, VkDevice device) {
    std::vector<VkFence> fences;
    for (int i = 0; i < fenceCount; i++) {
        VkFence fence;
        VkFenceCreateInfo fInfo{};
        fInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkResult fResult = vkCreateFence(device, &fInfo, nullptr, &fence);
        if (fResult != VK_SUCCESS) {
            WS::logError(fResult, "Fence: ");
            throw std::runtime_error("Fence Failed Creation");
        }
        fences.push_back(fence);
    }
    return fences;
}


void createImage(WS::Devices Devices, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, WS::Texture& Texture) {
    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent.depth = 1;
    createInfo.extent.width = width;
    createInfo.extent.height = height;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.format = format;
    createInfo.tiling = tiling;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.samples =  VK_SAMPLE_COUNT_1_BIT;
    createInfo.flags = 0;

    auto result = vkCreateImage(Devices.Device, &createInfo, nullptr, &Texture.Image);
    if (result != VK_SUCCESS) {
        WS::logError(result, "Texture: ");
        throw std::runtime_error("Texture Failed Creation");
    }

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(Devices.Device, Texture.Image, &memReq);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = WS::findMemoryType(Devices.PhysicalDevice, memReq.memoryTypeBits, properties);

    auto allocResult = vkAllocateMemory(Devices.Device, &allocInfo, nullptr, &Texture.Memory);
    if (allocResult != VK_SUCCESS) {
        WS::logError(allocResult, "Image Memory Allocation: ");
        throw std::runtime_error("Failed To Allocate Memory For Image");
    }
    vkBindImageMemory(Devices.Device, Texture.Image, Texture.Memory, 0);

}

void WS::createTextureImage(WS::Devices Devices, const char* path) {
    int width, height, channels;
    //stbi_uc* pixels = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    VkDeviceSize imageSize = width *height * 4;
    //if (!pixels) {
    //    throw std::runtime_error("Image Failed To Load");
    //}

    WS::BufferObjects StagingBuffer = WS::createStagingBuffer(Devices, imageSize);
    void* data;
    vkMapMemory(Devices.Device, StagingBuffer.Memory, 0, imageSize, 0, &data);
    //memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(Devices.Device, StagingBuffer.Memory);
    //stbi_image_free(pixels);

    WS::Texture Texture;
    
    //createImage(Devices, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, Texture);


}






