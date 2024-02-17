#include "vkInstance.h"

namespace Vulkan {
// ==================================================
// Private
// ==================================================
inline GraphicsBase GraphicsBase::singleton;

GraphicsBase::~GraphicsBase() {
    if (!instance) {
        return;
    }
    if (device) {
        WaitDeviceIdle();
        if (swapChain) {
            for (auto& i : callbacksDestroySwapChain) {
                i();
            }
            for (auto& i : swapChainImageViews) {
                if (i) {
                    vkDestroyImageView(device, i, nullptr);
                }
            }
            vkDestroySwapchainKHR(device, swapChain, nullptr);
        }
        for (auto& i : callbacksDestroyDevice) {
            i();
        }
        vkDestroyDevice(device, nullptr);
    }
    if (surface) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }
    if (debugUtilsMessenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessenger =
            reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (DestroyDebugUtilsMessenger) {
            DestroyDebugUtilsMessenger(instance, debugUtilsMessenger, nullptr);
        }
    }
    vkDestroyInstance(instance, nullptr);
}

void GraphicsBase::AddLayerOrExtension(std::vector<const char*>& container, const char* name) {
    for (auto& item : container) {
        if (strcmp(item, name) == 0) {
            return;
        }
    }
    container.push_back(name);
}

VkResult GraphicsBase::CreateDebugMessenger() {
    static PFN_vkDebugUtilsMessengerCallbackEXT DebugUtilsMessengerCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                                 VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                                                                 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                                                 void* pUserData) -> VkBool32 {
        std::cout << std::format("[DEBUG] {}\n\n", pCallbackData->pMessage);
        return VK_FALSE;
    };
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = DebugUtilsMessengerCallback};
    PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (CreateDebugUtilsMessenger) {
        VkResult result = CreateDebugUtilsMessenger(instance, &debugUtilsMessengerCreateInfo, nullptr, &debugUtilsMessenger);
        if (result) {
            std::cout << std::format("[GraphicsBase][ERROR] Failed to create a debug messenger! Error: {}({})\n", string_VkResult(result), int32_t(result));
        }
        return result;
    }
    std::cout << std::format("[GraphicsBase][ERROR] Failed to get the function pointer of vkCreateDebugUtilsMessengerEXT!\n");
    return VK_RESULT_MAX_ENUM;
}

VkResult GraphicsBase::GetQueueFamilyIndices(VkPhysicalDevice physicalDevice,
                                             bool enableGraphicsQueue,
                                             bool enableComputeQueue,
                                             uint32_t (&queueFamilyIndices)[3]) {
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    if (!queueFamilyCount) {
        return VK_RESULT_MAX_ENUM;
    }
    std::vector<VkQueueFamilyProperties> queueFamilyPropertieses(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyPropertieses.data());

    auto& [ig, ip, ic] = queueFamilyIndices;
    ig = ip = ic = VK_QUEUE_FAMILY_IGNORED;
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        VkBool32 supportGraphics = enableGraphicsQueue && queueFamilyPropertieses[i].queueFlags & VK_QUEUE_GRAPHICS_BIT,
                 supportPresentation = false,
                 supportCompute = enableComputeQueue && queueFamilyPropertieses[i].queueFlags & VK_QUEUE_COMPUTE_BIT;
        if (surface) {
            if (VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportPresentation)) {
                std::cout << std::format("[GraphicsBase][ERROR] Failed to determine if the queue family supports presentation! Error: {}({})\n", string_VkResult(result), int32_t(result));
                return result;
            }
        }
        if (supportGraphics && supportCompute) {
            if (supportPresentation) {
                ig = ip = ic = i;
                break;
            }
            if (ig != ic || ig == VK_QUEUE_FAMILY_IGNORED) {
                ig = ic = i;
            }
            if (!surface) {
                break;
            }
        }
        if (supportGraphics && ig == VK_QUEUE_FAMILY_IGNORED) {
            ig = i;
        }
        if (supportPresentation && ip == VK_QUEUE_FAMILY_IGNORED) {
            ip = i;
        }
        if (supportCompute && ic == VK_QUEUE_FAMILY_IGNORED) {
            ic = i;
        }
    }
    if (ig == VK_QUEUE_FAMILY_IGNORED && enableGraphicsQueue ||
        ip == VK_QUEUE_FAMILY_IGNORED && surface ||
        ic == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue) {
        return VK_RESULT_MAX_ENUM;
    }
    queueFamilyIndexGraphics = ig;
    queueFamilyIndexPresentation = ip;
    queueFamilyIndexCompute = ic;
    return VK_SUCCESS;
}

VkResult GraphicsBase::CreateSwapChainInternal() {
    if (VkResult result = vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapChain)) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to create a swap chain! Error: {}({})\n", string_VkResult(result), int32_t(result));
        return result;
    }
    // Create SwapChain images
    uint32_t swapChainImageCount;
    if (VkResult result = vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, nullptr)) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to get the count of swap chain images! Error: {}({})\n", string_VkResult(result), int32_t(result));
        return result;
    }
    swapChainImages.resize(swapChainImageCount);
    if (VkResult result = vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, swapChainImages.data())) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to get the swap chain images! Error: {}({})\n", string_VkResult(result), int32_t(result));
        return result;
    }
    // Create image views
    swapChainImageViews.resize(swapChainImageCount);
    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = swapChainCreateInfo.imageFormat,
        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
    for (size_t i = 0; i < swapChainImageCount; i++) {
        imageViewCreateInfo.image = swapChainImages[i];
        if (VkResult result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &swapChainImageViews[i])) {
            std::cout << std::format("[GraphicsBase][ERROR] Failed to create the image view of swap chain image! Error: {}({})\n", string_VkResult(result), int32_t(result));
            return result;
        }
    }
    return VK_SUCCESS;
}

// ==================================================
// Public
// ==================================================
GraphicsBase& GraphicsBase::GetInstance() {
    return singleton;
}

uint32_t GraphicsBase::GetApiVersion() const {
    return apiVersion;
}

VkInstance GraphicsBase::GetInstanceHandle() const {
    return instance;
}

const std::vector<const char*>& GraphicsBase::GetInstanceLayers() const {
    return instanceLayers;
}

const std::vector<const char*>& GraphicsBase::GetInstanceExtensions() const {
    return instanceExtensions;
}

VkSurfaceKHR GraphicsBase::GetSurface() const {
    return surface;
}

VkPhysicalDevice GraphicsBase::GetPhysicalDevice() const {
    return physicalDevice;
}

VkPhysicalDeviceProperties GraphicsBase::GetPhysicalDeviceProperties() const {
    return physicalDeviceProperties;
}

VkPhysicalDeviceMemoryProperties GraphicsBase::GetPhysicalDeviceMemoryProperties() const {
    return physicalDeviceMemoryProperties;
}

VkPhysicalDevice GraphicsBase::GetAvailablePhysicalDevice(uint32_t index) const {
    return availablePhysicalDevices[index];
}

uint32_t GraphicsBase::GetAvailablePhysicalDeviceCount() const {
    return uint32_t(availablePhysicalDevices.size());
}

VkDevice GraphicsBase::GetDevice() const {
    return device;
}

uint32_t GraphicsBase::GetQueueFamilyIndexGraphics() const {
    return queueFamilyIndexGraphics;
}

uint32_t GraphicsBase::GetQueueFamilyIndexPresentation() const {
    return queueFamilyIndexPresentation;
}

uint32_t GraphicsBase::GetQueueFamilyIndexCompute() const {
    return queueFamilyIndexCompute;
}

VkQueue GraphicsBase::GetQueueGraphics() const {
    return queueGraphics;
}

VkQueue GraphicsBase::GetQueuePresentation() const {
    return queuePresentation;
}

VkQueue GraphicsBase::GetQueueCompute() const {
    return queueCompute;
}

const std::vector<const char*>& GraphicsBase::GetDeviceExtensions() const {
    return deviceExtensions;
}

const VkFormat& GraphicsBase::GetAvailableSurfaceFormat(uint32_t index) const {
    return availableSurfaceFormats[index].format;
}

const VkColorSpaceKHR& GraphicsBase::GetAvailableSurfaceColorSpace(uint32_t index) const {
    return availableSurfaceFormats[index].colorSpace;
}

uint32_t GraphicsBase::GetAvailableSurfaceFormatCount() const {
    return uint32_t(availableSurfaceFormats.size());
}

VkSwapchainKHR GraphicsBase::GetSwapChain() const {
    return swapChain;
}

VkImage GraphicsBase::GetSwapChainImage(uint32_t index) const {
    return swapChainImages[index];
}

VkImageView GraphicsBase::GetSwapChainImageView(uint32_t index) const {
    return swapChainImageViews[index];
}

uint32_t GraphicsBase::GetSwapChainImageCount() const {
    return uint32_t(swapChainImages.size());
}

const VkSwapchainCreateInfoKHR& GraphicsBase::GetSwapChainCreateInfo() const {
    return swapChainCreateInfo;
}

VkResult GraphicsBase::GetSurfaceFormats() {
    uint32_t surfaceFormatCount;
    if (VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr)) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to get the count of surface formats! Error: {}({})\n", string_VkResult(result), int32_t(result));
        return result;
    }
    if (!surfaceFormatCount) {
        throw std::runtime_error("[GraphicsBase][ERROR] Failed to find any surface format!");
    }
    availableSurfaceFormats.resize(surfaceFormatCount);
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, availableSurfaceFormats.data());
    if (result) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to get the surface formats! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

VkResult GraphicsBase::UseLatestApiVersion() {
    if (vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion") != nullptr) {
        return vkEnumerateInstanceVersion(&apiVersion);
    }
    return VK_SUCCESS;
}

void GraphicsBase::PushInstanceLayer(const char* name) {
    AddLayerOrExtension(instanceLayers, name);
}

void GraphicsBase::PushInstanceExtension(const char* name) {
    AddLayerOrExtension(instanceExtensions, name);
}

VkResult GraphicsBase::CreateInstance(const void* pNext, VkInstanceCreateFlags flags) {
#ifndef NDEBUG
    PushInstanceLayer("VK_LAYER_KHRONOS_validation");
    PushInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    VkApplicationInfo appInfo = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO, .apiVersion = apiVersion};
    VkInstanceCreateInfo instanceCreateInfo = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                               .pNext = pNext,
                                               .flags = flags,
                                               .pApplicationInfo = &appInfo,
                                               .enabledLayerCount = uint32_t(instanceLayers.size()),
                                               .ppEnabledLayerNames = instanceLayers.data(),
                                               .enabledExtensionCount = uint32_t(instanceExtensions.size()),
                                               .ppEnabledExtensionNames = instanceExtensions.data()};
    if (VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance)) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to create a vulkan instance! Error: {}({})\n", string_VkResult(result), int32_t(result));
        return result;
    }
    // if success to create vulkan instance, print the version of Vulkan API
    std::cout << std::format("[INFO] Vulkan API Version: {}.{}.{}\n",
                             VK_VERSION_MAJOR(apiVersion),
                             VK_VERSION_MINOR(apiVersion),
                             VK_VERSION_PATCH(apiVersion));
#ifndef NDEBUG
    // create debug messenger
    CreateDebugMessenger();
#endif
    return VK_SUCCESS;
}

VkResult GraphicsBase::CheckInstaneLayers(std::span<const char*> layersToCheck) const {
    uint32_t layerCount;
    std::vector<VkLayerProperties> availableLayers;
    if (VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr)) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to get the count of instance layers!\n");
        return result;
    }
    if (layerCount) {
        availableLayers.resize(layerCount);
        if (VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data())) {
            std::cout << std::format("[GraphicsBase][ERROR] Failed to enumerate instance layer properties! Error: {}({})\n", string_VkResult(result), int32_t(result));
            return result;
        }
        for (auto& i : layersToCheck) {
            bool found = false;
            for (auto& j : availableLayers) {
                if (!strcmp(i, j.layerName)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                i = nullptr;
            }
        }
    } else {
        for (auto& i : layersToCheck) {
            i = nullptr;
        }
    }
    return VK_SUCCESS;
}

void GraphicsBase::InstanceLayers(const std::vector<const char*>& layerNames) {
    instanceLayers = layerNames;
}

VkResult GraphicsBase::CheckInstaneExtensions(std::span<const char*> extensionsToCheck, const char* layerName) const {
    uint32_t extensionCount;
    std::vector<VkExtensionProperties> availableExtensions;
    if (VkResult result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, nullptr)) {
        layerName ? std::cout << std::format("[GraphicsBase][ERROR] Failed to get the count of instance extensions! Layer name:{}\n", layerName)
                  : std::cout << std::format("[GraphicsBase][ERROR] Failed to get the count of instance extensions!\n");
        return result;
    }
    if (extensionCount) {
        availableExtensions.resize(extensionCount);
        if (VkResult result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, availableExtensions.data())) {
            std::cout << std::format("[GraphicsBase][ERROR] Failed to enumerate instance extension properties! Error: {}({})\n", string_VkResult(result), int32_t(result));
            return result;
        }
        for (auto& i : extensionsToCheck) {
            bool found = false;
            for (auto& j : availableExtensions) {
                if (!strcmp(i, j.extensionName)) {
                    found = true;
                    break;
                }
            }
            if (!found)
                i = nullptr;
        }
    } else {
        for (auto& i : extensionsToCheck) {
            i = nullptr;
        }
    }
    return VK_SUCCESS;
}

void GraphicsBase::InstanceExtensions(const std::vector<const char*>& extensionNames) {
    instanceExtensions = extensionNames;
}

void GraphicsBase::Surface(VkSurfaceKHR surface) {
    if (!this->surface)
        this->surface = surface;
}

void GraphicsBase::PushDeviceExtension(const char* extensionName) {
    AddLayerOrExtension(deviceExtensions, extensionName);
}

VkResult GraphicsBase::GetPhysicalDevices() {
    uint32_t deviceCount;
    if (VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr)) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to get the count of physical devices! Error: {}({})\n", string_VkResult(result), int32_t(result));
        return result;
    }
    if (!deviceCount) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to find any physical device supports vulkan!\n");
        abort();
    }
    availablePhysicalDevices.resize(deviceCount);
    VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, availablePhysicalDevices.data());
    if (result) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to enumerate physical devices! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

VkResult
GraphicsBase::DeterminePhysicalDevice(uint32_t deviceIndex, bool enableGraphicsQueue, bool enableComputeQueue) {
    static constexpr uint32_t notFound = INT32_MAX;
    struct queueFamilyIndexCombination {
        uint32_t graphics = VK_QUEUE_FAMILY_IGNORED;
        uint32_t presentation = VK_QUEUE_FAMILY_IGNORED;
        uint32_t compute = VK_QUEUE_FAMILY_IGNORED;
    };
    static std::vector<queueFamilyIndexCombination> queueFamilyIndexCombinations(availablePhysicalDevices.size());
    auto& [ig, ip, ic] = queueFamilyIndexCombinations[deviceIndex];

    if (ig == notFound && enableGraphicsQueue ||
        ip == notFound && surface ||
        ic == notFound && enableComputeQueue) {
        return VK_RESULT_MAX_ENUM;
    }
    if (ig == VK_QUEUE_FAMILY_IGNORED && enableGraphicsQueue ||
        ip == VK_QUEUE_FAMILY_IGNORED && surface ||
        ic == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue) {
        uint32_t indices[3];
        VkResult result = GetQueueFamilyIndices(availablePhysicalDevices[deviceIndex], enableGraphicsQueue, enableComputeQueue, indices);
        if (result == VK_SUCCESS ||
            result == VK_RESULT_MAX_ENUM) {
            if (enableGraphicsQueue)
                ig = indices[0] & INT32_MAX;
            if (surface)
                ip = indices[1] & INT32_MAX;
            if (enableComputeQueue)
                ic = indices[2] & INT32_MAX;
        }
        if (result) {
            return result;
        }
    } else {
        queueFamilyIndexGraphics = enableGraphicsQueue ? ig : VK_QUEUE_FAMILY_IGNORED;
        queueFamilyIndexPresentation = surface ? ip : VK_QUEUE_FAMILY_IGNORED;
        queueFamilyIndexCompute = enableComputeQueue ? ic : VK_QUEUE_FAMILY_IGNORED;
    }
    physicalDevice = availablePhysicalDevices[deviceIndex];
    return VK_SUCCESS;
}

VkResult GraphicsBase::CreateDevice(const void* pNext, VkDeviceCreateFlags flags) {
    float queuePriority = 1.f;
    VkDeviceQueueCreateInfo queueCreateInfo[3] = {
        {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
         .queueFamilyIndex = queueFamilyIndexGraphics,
         .queueCount = 1,
         .pQueuePriorities = &queuePriority},
        {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
         .queueFamilyIndex = queueFamilyIndexPresentation,
         .queueCount = 1,
         .pQueuePriorities = &queuePriority},
        {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
         .queueFamilyIndex = queueFamilyIndexCompute,
         .queueCount = 1,
         .pQueuePriorities = &queuePriority}};
    uint32_t queueCreateInfoCount = 0;
    if (queueFamilyIndexGraphics != VK_QUEUE_FAMILY_IGNORED) {
        queueCreateInfo[queueCreateInfoCount++].queueFamilyIndex = queueFamilyIndexGraphics;
    }
    if (queueFamilyIndexPresentation != VK_QUEUE_FAMILY_IGNORED &&
        queueFamilyIndexGraphics != queueFamilyIndexPresentation) {
        queueCreateInfo[queueCreateInfoCount++].queueFamilyIndex = queueFamilyIndexPresentation;
    }
    if (queueFamilyIndexCompute != VK_QUEUE_FAMILY_IGNORED &&
        queueFamilyIndexCompute != queueFamilyIndexGraphics &&
        queueFamilyIndexCompute != queueFamilyIndexPresentation) {
        queueCreateInfo[queueCreateInfoCount++].queueFamilyIndex = queueFamilyIndexCompute;
    }
    // Get the physical device features
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
    // Create the logical device
    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = pNext,
        .flags = flags,
        .queueCreateInfoCount = queueCreateInfoCount,
        .pQueueCreateInfos = queueCreateInfo,
        .enabledExtensionCount = uint32_t(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = &physicalDeviceFeatures};
    if (VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device)) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to create a logical device! Error: {}({})\n", string_VkResult(result), int32_t(result));
        return result;
    }
    // Get the queue handles
    if (queueFamilyIndexGraphics != VK_QUEUE_FAMILY_IGNORED) {
        vkGetDeviceQueue(device, queueFamilyIndexGraphics, 0, &queueGraphics);
    }
    if (queueFamilyIndexPresentation != VK_QUEUE_FAMILY_IGNORED) {
        vkGetDeviceQueue(device, queueFamilyIndexPresentation, 0, &queuePresentation);
    }
    if (queueFamilyIndexCompute != VK_QUEUE_FAMILY_IGNORED) {
        vkGetDeviceQueue(device, queueFamilyIndexCompute, 0, &queueCompute);
    }
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);
    // Print the physical device's name
    std::cout << std::format("[INFO] Physical Device: {}\n", physicalDeviceProperties.deviceName);
    // call callbacks
    for (auto& i : callbacksCreateDevice) {
        i();
    }
    return VK_SUCCESS;
}

VkResult GraphicsBase::CheckDeviceExtensions(std::span<const char*> extensionsToCheck, const char* layerName) const {
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
}

void GraphicsBase::DeviceExtensions(const std::vector<const char*>& extensionNames) {
    deviceExtensions = extensionNames;
}

VkResult GraphicsBase::SetSurfaceFormat(VkSurfaceFormatKHR surfaceFormat) {
    bool formatIsAvailable = false;
    // colorspace default is VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, is SRGB, a standard colorspace
    if (!surfaceFormat.format) {
        // If specified colorspace and not specified format
        for (auto& i : availableSurfaceFormats) {
            if (i.colorSpace == surfaceFormat.colorSpace) {
                swapChainCreateInfo.imageFormat = i.format;
                swapChainCreateInfo.imageColorSpace = i.colorSpace;
                formatIsAvailable = true;
                break;
            }
        }
    } else {
        for (auto& i : availableSurfaceFormats) {
            if (i.format == surfaceFormat.format && i.colorSpace == surfaceFormat.colorSpace) {
                swapChainCreateInfo.imageFormat = i.format;
                swapChainCreateInfo.imageColorSpace = i.colorSpace;
                formatIsAvailable = true;
                break;
            }
        }
    }
    if (!formatIsAvailable) {
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }
    // This function maybe called when rendering(eg. switch to HDR), so recreate the swap chain is necessary
    if (swapChain) {
        return RecreateSwapChain();
    }
    return VK_SUCCESS;
}

VkResult GraphicsBase::CreateSwapChain(bool limitFrameRate, const void* pNext, VkSwapchainCreateFlagsKHR flags) {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities)) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to get the surface capabilities! Error: {}({})\n", string_VkResult(result), int32_t(result));
        return result;
    }
    swapChainCreateInfo.minImageCount = surfaceCapabilities.minImageCount + (surfaceCapabilities.maxImageCount > surfaceCapabilities.minImageCount);
    swapChainCreateInfo.imageExtent = surfaceCapabilities.currentExtent.width == -1 ? VkExtent2D{surfaceCapabilities.minImageExtent.width, surfaceCapabilities.minImageExtent.height} : surfaceCapabilities.currentExtent;
    swapChainCreateInfo.imageArrayLayers = 1; // view count, default is 1
    swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    //== Set the composite alpha
    //== If the composite alpha is supported, use VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
    //== Else, use the first supported composite alpha
    if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
        swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    } else {
        for (size_t i = 0; i < 4; i++) {
            if (surfaceCapabilities.supportedCompositeAlpha & (1 << i)) {
                swapChainCreateInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR(surfaceCapabilities.supportedCompositeAlpha & 1 << i);
                break;
            }
        }
    }
    // SwapChain imageUsage
    // SRC: read from the image, such as user can screenshot vk app window when the image usage is VK_IMAGE_USAGE_TRANSFER_SRC_BIT
    // DST: write to the image, such as developer can clear the window color when the image usage is VK_IMAGE_USAGE_TRANSFER_DST_BIT
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        swapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        swapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    } else {
        std::cout << "[GraphicsBase][WARNING] VK_IMAGE_USAGE_TRANSFER_DST_BIT is not supported by the surface!\n";
    }
    if (availableSurfaceFormats.empty()) {
        if (VkResult result = GetSurfaceFormats()) {
            return result;
        }
    }
    // if the surface format is not specified, call SetSurfaceFormat() function to set the default format
    if (!swapChainCreateInfo.imageFormat) {
        if (SetSurfaceFormat({VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}) &&
            SetSurfaceFormat({VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})) {
            // if the surface format is not specified and the default format is not available
            // use the first available format
            swapChainCreateInfo.imageFormat = availableSurfaceFormats[0].format;
            swapChainCreateInfo.imageColorSpace = availableSurfaceFormats[0].colorSpace;
            std::cout << std::format("[GraphicsBase][WARNING] Failed to select a four-component UNORM surface format! Use the first available format!\n");
        }
    }
    // present mode
    uint32_t surfacePresentModeCount;
    if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, nullptr)) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to get the count of surface present modes! Error: {}({})\n", string_VkResult(result), int32_t(result));
        return result;
    }
    if (!surfacePresentModeCount) {
        throw std::runtime_error("[GraphicsBase][ERROR] Failed to find any surface present mode!");
    }
    std::vector<VkPresentModeKHR> surfacePresentModes(surfacePresentModeCount);
    if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, surfacePresentModes.data())) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to get the surface present modes! Error: {}({})\n", string_VkResult(result), int32_t(result));
        return result;
    }
    // default present mode is VK_PRESENT_MODE_FIFO_KHR
    // if limitFrameRate is false, use VK_PRESENT_MODE_MAILBOX_KHR
    swapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (!limitFrameRate) {
        for (size_t i = 0; i < surfacePresentModeCount; i++) {
            if (surfacePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                swapChainCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                break;
            }
        }
    }
    // rest of the swap chain create info
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.pNext = pNext;
    swapChainCreateInfo.flags = flags;
    swapChainCreateInfo.surface = surface;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.clipped = VK_TRUE;
    // Create the swap chain
    if (VkResult result = CreateSwapChainInternal()) {
        return result;
    }
    // Call the callbacks
    for (auto& i : callbacksCreateSwapChain) {
        i();
    }
    return VK_SUCCESS;
}

VkResult GraphicsBase::RecreateSwapChain() {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities)) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to get the surface capabilities! Error: {}({})\n", string_VkResult(result), int32_t(result));
        return result;
    }
    if (surfaceCapabilities.currentExtent.width == 0 ||
        surfaceCapabilities.currentExtent.height == 0)
        return VK_SUCCESS;
    swapChainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapChainCreateInfo.oldSwapchain = swapChain;
    // before destroy the old swap chain, wait for the graphics queue to be idle
    VkResult result = vkQueueWaitIdle(queueGraphics);
    if (result == VK_SUCCESS && queueGraphics != queuePresentation) {
        result = vkQueueWaitIdle(queuePresentation);
    }
    if (result) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to wait for the graphics queue to be idle! Error: {}({})\n", string_VkResult(result), int32_t(result));
        return result;
    }
    // destroy the old swap chain
    for (auto& i : swapChainImageViews) {
        if (i) {
            vkDestroyImageView(device, i, nullptr);
        }
    }
    swapChainImageViews.resize(0);
    if (result = CreateSwapChainInternal()) {
        return result;
    }
    // Call the callbacks
    for (auto& i : callbacksDestroySwapChain) {
        i();
    }
    return VK_SUCCESS;
}

void GraphicsBase::AddCallbackCreateSwapChain(void (*callback)()) {
    callbacksCreateSwapChain.push_back(callback);
}

void GraphicsBase::AddCallbackDestroySwapChain(void (*callback)()) {
    callbacksDestroySwapChain.push_back(callback);
}

void GraphicsBase::AddCallbackCreateDevice(void (*callback)()) {
    callbacksCreateDevice.push_back(callback);
}

void GraphicsBase::AddCallbackDestroyDevice(void (*callback)()) {
    callbacksDestroyDevice.push_back(callback);
}

VkResult GraphicsBase::RecreateDevice(const void* pNext, VkDeviceCreateFlags flags) {
    if (VkResult result = WaitDeviceIdle()) {
        return result;
    }
    if (swapChain) {
        for (auto& i : callbacksDestroyDevice) {
            i();
        }
        for (auto& i : swapChainImageViews) {
            if (i) {
                vkDestroyImageView(device, i, nullptr);
            }
        }
        swapChainImageViews.resize(0);
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        swapChain = VK_NULL_HANDLE;
        swapChainCreateInfo = {};
    }
    for (auto& i : callbacksDestroyDevice) {
        i();
    }
    if (device) {
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }
    return CreateDevice(pNext, flags);
}

VkResult GraphicsBase::WaitDeviceIdle() {
    VkResult result = vkDeviceWaitIdle(device);
    if (result) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to wait for the device to be idle! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

void GraphicsBase::Terminate() {
    this->~GraphicsBase();
    instance = VK_NULL_HANDLE;
    physicalDevice = VK_NULL_HANDLE;
    device = VK_NULL_HANDLE;
    surface = VK_NULL_HANDLE;
    swapChain = VK_NULL_HANDLE;
    swapChainImages.resize(0);
    swapChainImageViews.resize(0);
    swapChainCreateInfo = {};
    debugUtilsMessenger = VK_NULL_HANDLE;
}

} // namespace Vulkan
