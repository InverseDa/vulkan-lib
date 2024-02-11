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
    // TODO: optimize the code
}

VkResult GraphicsBase::CreateSwapChainInternal() {}

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
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
}

VkResult GraphicsBase::CreateDevice(const void* pNext, VkDeviceCreateFlags flags) {
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
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
