#ifndef VULKAN_LIB_VKCORE_H
#define VULKAN_LIB_VKCORE_H

#include "vulkan/vulkan.h"
#include "vulkan/vk_enum_string_helper.h"
#include "macro.h"
#include "type/vkResult.h"
#include "core/ostream.h"

#include <vector>
#include <span>
#include <iostream>
#include <functional>

#ifdef __APPLE__
#include "core/format.h"
#else
#include <format>
#endif

namespace Vulkan {
class Context {
  private:
    // Vulkan api version
    uint32_t apiVersion = VK_API_VERSION_1_0;
    // Singleton
    static Context singleton;
    // Constructor and destructor
    Context() = default;
    Context(Context&&) = delete;
    ~Context();

    // vk instance and its layers and extensions
    VkInstance instance;
    std::vector<const char*> instanceLayers;
    std::vector<const char*> instanceExtensions;
    static void AddLayerOrExtension(std::vector<const char*>& container, const char* name);

    // vk debug messenger
    VkDebugUtilsMessengerEXT debugUtilsMessenger;
    ResultType CreateDebugMessenger();

    // vk window surface
    VkSurfaceKHR surface;

    // vk physical device
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    std::vector<VkPhysicalDevice> availablePhysicalDevices;
    // vk logical device
    VkDevice device;
    //// three queue families: graphics, presentation, and compute
    uint32_t queueFamilyIndexGraphics = VK_QUEUE_FAMILY_IGNORED;
    uint32_t queueFamilyIndexPresentation = VK_QUEUE_FAMILY_IGNORED;
    uint32_t queueFamilyIndexCompute = VK_QUEUE_FAMILY_IGNORED;
    VkQueue queueGraphics;
    VkQueue queuePresentation;
    VkQueue queueCompute;
    std::vector<const char*> deviceExtensions;
    // functions to check whether the physical device meets the required queue family type, and return the
    // corresponding queue family index to queueFamilyIndices. When the execution is successful, the index is directly
    // written to the corresponding member variable.
    ResultType GetQueueFamilyIndices(VkPhysicalDevice physicalDevice, bool enableGraphicsQueue, bool enableComputeQueue, uint32_t (&queueFamilyIndices)[3]);

    // vk swap chain
    std::vector<VkSurfaceFormatKHR> availableSurfaceFormats;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    // save the swap chain's create info to recreate the swap chain conveniently
    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    ResultType CreateSwapChainInternal();
    // callback function vector
    std::vector<std::function<void()>> callbacksCreateSwapChain;
    std::vector<std::function<void()>> callbacksDestroySwapChain;
    std::vector<std::function<void()>> callbacksCreateDevice;
    std::vector<std::function<void()>> callbacksDestroyDevice;
    // current image index
    uint32_t currentImageIndex = 0;

  public:
    static Context& GetInstance();
    // Getter
    uint32_t GetApiVersion() const;
    //// vk instance and its layers and extensions
    VkInstance GetInstanceHandle() const;
    const std::vector<const char*>& GetInstanceLayers() const;
    const std::vector<const char*>& GetInstanceExtensions() const;
    //// vk window surface
    VkSurfaceKHR GetSurface() const;
    //// vk physical device and its properties
    VkPhysicalDevice GetPhysicalDevice() const;
    VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const;
    VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const;
    VkPhysicalDevice GetAvailablePhysicalDevice(uint32_t index) const;
    uint32_t GetAvailablePhysicalDeviceCount() const;
    //// vk logical device and its queues
    VkDevice GetDevice() const;
    uint32_t GetQueueFamilyIndexGraphics() const;
    uint32_t GetQueueFamilyIndexPresentation() const;
    uint32_t GetQueueFamilyIndexCompute() const;
    VkQueue GetQueueGraphics() const;
    VkQueue GetQueuePresentation() const;
    VkQueue GetQueueCompute() const;
    const std::vector<const char*>& GetDeviceExtensions() const;
    //// vk swap chain
    const VkFormat& GetAvailableSurfaceFormat(uint32_t index) const;
    const VkColorSpaceKHR& GetAvailableSurfaceColorSpace(uint32_t index) const;
    uint32_t GetAvailableSurfaceFormatCount() const;
    VkSwapchainKHR GetSwapChain() const;
    VkImage GetSwapChainImage(uint32_t index) const;
    VkImageView GetSwapChainImageView(uint32_t index) const;
    uint32_t GetSwapChainImageCount() const;
    const VkSwapchainCreateInfoKHR& GetSwapChainCreateInfo() const;
    ResultType GetSurfaceFormats();

    // Functions that use the latest api version
    ResultType UseLatestApiVersion();

    // Functions that before the instance is created
    void PushInstanceLayer(const char* name);
    void PushInstanceExtension(const char* name);
    // Functions that create the instance
    ResultType CreateInstance(const void* pNext = nullptr, VkInstanceCreateFlags flags = 0);
    // Functions when the instance is failed to create
    ResultType CheckInstanceLayers(std::span<const char*> layersToCheck) const;
    void InstanceLayers(const std::vector<const char*>& layerNames);
    ResultType CheckInstanceExtensions(std::span<const char*> extensionsToCheck, const char* layerName = nullptr) const;
    void InstanceExtensions(const std::vector<const char*>& extensionNames);

    // Functions that before chose the physical device
    void Surface(VkSurfaceKHR surface);

    // Functions that before the logical device is created
    void PushDeviceExtension(const char* extensionName);
    // Functions that get the physical device
    ResultType GetPhysicalDevices();
    // Functions that determine the physical device and use `GetQueueFamilyIndices` to get the queue family indices
    ResultType
    DeterminePhysicalDevice(uint32_t deviceIndex = 0, bool enableGraphicsQueue = true, bool enableComputeQueue = true);
    // Functions that create the logical device and get its queues
    ResultType CreateDevice(const void* pNext = nullptr, VkDeviceCreateFlags flags = 0);
    // Functions when the logical device is failed to create
    ResultType CheckDeviceExtensions(std::span<const char*> extensionsToCheck, const char* layerName = nullptr) const;
    void DeviceExtensions(const std::vector<const char*>& extensionNames);
    // Functions that recreate device
    ResultType RecreateDevice(const void* pNext = nullptr, VkDeviceCreateFlags flags = 0);
    // Functions that wait device until idle
    ResultType WaitDeviceIdle();

    // Functions that set the surface format
    ResultType SetSurfaceFormat(VkSurfaceFormatKHR surfaceFormat);
    // Functions that create the swap chain
    ResultType
    CreateSwapChain(bool limitFrameRate = true, const void* pNext = nullptr, VkSwapchainCreateFlagsKHR flags = 0);
    // Functions that recreate the swap chain
    ResultType RecreateSwapChain();
    // Callback functions
    void AddCallbackCreateSwapChain(std::function<void()>& callback);
    void AddCallbackDestroySwapChain(std::function<void()>& callback);
    void AddCallbackCreateDevice(std::function<void()>& callback);
    void AddCallbackDestroyDevice(std::function<void()>& callback);
    // Functions that swap the image
    ResultType SwapImage(VkSemaphore imageIsAvailableSem);
    uint32_t GetCurrentImageIndex() const;
    // Functions that submit the command buffer
    //// graphics
    ////// this function is used to submit the command buffer to the graphics queue include the data transfer commands
    ////// so the semaphore is not needed
    ResultType SubmitCommandBufferGraphics(VkSubmitInfo& submitInfo, VkFence fence = VK_NULL_HANDLE) const;
    ////// this function is used to submit the command buffer to the graphics queue in rendering loop case
    ResultType SubmitCommandBufferGraphics(VkCommandBuffer commandBuffer, VkSemaphore imageIsAvailableSem = VK_NULL_HANDLE, VkSemaphore renderingIsFinishedSem = VK_NULL_HANDLE, VkFence fence = VK_NULL_HANDLE) const;
    ////// this function is used to submit the command buffer to the graphics queue only use fence
    ResultType SubmitCommandBufferGraphics(VkCommandBuffer commandBuffer, VkFence fence = VK_NULL_HANDLE) const;

    ResultType SubmitCommandBufferCompute(VkSubmitInfo& submitInfo, VkFence fence = VK_NULL_HANDLE) const;
    ResultType SubmitCommandBufferCompute(VkCommandBuffer commandBuffer, VkFence fence = VK_NULL_HANDLE) const;
    // Functions that present the image
    ResultType PresentImage(VkPresentInfoKHR& presentInfo);
    ResultType PresentImage(VkSemaphore renderingIsFinishedSem = VK_NULL_HANDLE);

    // Functions that terminate vulkan instance when the program is running
    void Terminate();
};
} // namespace Vulkan

#endif