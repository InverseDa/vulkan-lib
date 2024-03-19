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
#include "command/command.hpp"

namespace ida {
class Context final {
  public:
    static void Init(std::vector<const char*>& extensions, GetSurfaceCallback);
    static void Quit();

    void InitVulkan(int windowWidth, int windowHeight);

    static Context& GetInstance();

    struct QueueInfo final {
        std::optional<std::uint32_t> graphicsIndex;
        std::optional<std::uint32_t> presentIndex;

        operator bool() {
            return graphicsIndex.has_value() && presentIndex.has_value();
        }
    } queueInfo;

    vk::Instance instance;
    vk::PhysicalDevice phyDevice;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    std::unique_ptr<IdaSwapChain> swapChain;
    std::unique_ptr<CommandMgr> commandMgr;
    vk::Sampler sampler;

    ~Context();

    friend class IdaWindow;

  private:
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
    void GetSurface();

    vk::Instance CreateInstance(std::vector<const char*>& extensions);
    vk::PhysicalDevice PickupPhysicalDevice();
    vk::Device CreateDevice(vk::SurfaceKHR);

    void QueryQueueInfo(vk::SurfaceKHR);
};
} // namespace ida

#endif // VULKAN_LIB_CONTEXT_HPP
