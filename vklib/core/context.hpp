#ifndef VULKAN_LIB_CONTEXT_HPP
#define VULKAN_LIB_CONTEXT_HPP

#include "vulkan/vulkan.hpp"
#include <memory>
#include <cassert>
#include <iostream>
#include <optional>

#include "tools.hpp"
#include "swapchain/swapchain.hpp"
#include "render/renderer.hpp"

namespace ida {
struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

struct QueueFamilyIndices final {
    std::optional<std::uint32_t> graphicsIndex;
    std::optional<std::uint32_t> presentIndex;

    operator bool() {
        return graphicsIndex.has_value() && presentIndex.has_value();
    }
};

class Context final {
  public:
    friend class IdaWindow;

    static void Init(std::vector<const char*>& extensions, GetSurfaceCallback);
    static void Quit();
    static Context& GetInstance();
    vk::SurfaceKHR GetSurface() { return surface_; }
    void InitVulkan(int windowWidth, int windowHeight);
    SwapChainSupportDetails QuerySwapChainSupport();
    QueueFamilyIndices QueryQueueFamily(vk::SurfaceKHR);
    vk::Format QuerySupportedFormat(const std::vector<vk::Format>&, vk::ImageTiling, vk::FormatFeatureFlags);

    void CreateImageWithInfo(const vk::ImageCreateInfo&, vk::MemoryPropertyFlags, vk::Image&, vk::DeviceMemory&);

    ~Context();

    vk::Instance instance;
    vk::PhysicalDevice phyDevice;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    std::unique_ptr<IdaSwapChain> swapChain;
    vk::Sampler sampler;
    vk::CommandPool commandPool;

  private:
    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    static Context* instance_;
    vk::SurfaceKHR surface_ = nullptr;
    GetSurfaceCallback getSurfaceCb_ = nullptr;

    Context(std::vector<const char*>& extensions, GetSurfaceCallback);
    void InitRenderProcess();
    void InitSwapChain(int windowWidth, int windowHeight);
    void InitGraphicsPipeline();
    void InitCommandPool();
    void InitShaderModules();
    void InitSampler();
    uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    vk::Instance CreateInstance(std::vector<const char*>& extensions);
    vk::PhysicalDevice PickupPhysicalDevice();
    vk::Device CreateDevice(vk::SurfaceKHR);
};
} // namespace ida

#endif // VULKAN_LIB_CONTEXT_HPP
