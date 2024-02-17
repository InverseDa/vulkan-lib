#include "vkInstance.h"

namespace Vulkan {
// ==================================================
// Private
// ==================================================
inline GraphicsBase GraphicsBase::singleton;

GraphicsBase::~GraphicsBase() {}

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
        std::cout << std::format("{}\n\n", pCallbackData->pMessage);
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
            std::cout << std::format("[GraphicsBase][ERROR] Failed to create a debug messenger! Error code: {}\n", int32_t(result));
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
                std::cout << std::format("[GraphicsBase][ERROR] Failed to determine if the queue family supports presentation! Error code: {}\n", int32_t(result));
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
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
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
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
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
        std::cout << std::format("[GraphicsBase][ERROR] Failed to create a vulkan instance! Error code: {}\n", int32_t(result));
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
            std::cout << std::format("[GraphicsBase][ERROR] Failed to enumerate instance layer properties! Error code: {}\n", int32_t(result));
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
            std::cout << std::format("[GraphicsBase][ERROR] Failed to enumerate instance extension properties! Error code: {}\n", int32_t(result));
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
        std::cout << std::format("[GraphicsBase][ERROR] Failed to get the count of physical devices! Error code: {}\n", int32_t(result));
        return result;
    }
    if (!deviceCount) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to find any physical device supports vulkan!\n");
        abort();
    }
    availablePhysicalDevices.resize(deviceCount);
    VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, availablePhysicalDevices.data());
    if (result) {
        std::cout << std::format("[GraphicsBase][ERROR] Failed to enumerate physical devices! Error code: {}\n", int32_t(result));
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
        std::cout << std::format("[GraphicsBase][ERROR] Failed to create a logical device! Error code: {}\n", int32_t(result));
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

    return VK_SUCCESS;
}

VkResult GraphicsBase::CheckDeviceExtensions(std::span<const char*> extensionsToCheck, const char* layerName) const {
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
}

void GraphicsBase::DeviceExtensions(const std::vector<const char*>& extensionNames) {
    deviceExtensions = extensionNames;
}

VkResult GraphicsBase::SetSurfaceFormat(VkSurfaceFormatKHR surfaceFormat) {
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
}

VkResult GraphicsBase::CreateSwapChain(bool limitFrameRate, const void* pNext, VkSwapchainCreateFlagsKHR flags) {
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
}

VkResult GraphicsBase::RecreateSwapChain() {
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
}
} // namespace Vulkan
