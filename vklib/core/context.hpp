#ifndef VULKAN_LIB_CONTEXT_HPP
#define VULKAN_LIB_CONTEXT_HPP

#include "vulkan/vulkan.hpp"
#include <memory>
#include <cassert>
#include <iostream>
#include <optional>

#include "tools.hpp"
#include "swapchain/swapchain.hpp"
#include "render/render_process.hpp"
#include "render/renderer.hpp"
#include "cmd/command_mgr.hpp"
#include "shader/shader.hpp"

namespace Vklib {
class Context final {
  public:
    using GetSurfaceCallback = std::function<vk::SurfaceKHR(vk::Instance)>;

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
    std::unique_ptr<Swapchain> swapchain;
    std::unique_ptr<RenderProcess> renderProcess;
    std::unique_ptr<CommandMgr> commandMgr;
    std::unique_ptr<Shader> shader;
    vk::Sampler sampler;

    void ResizeSwapchainImage(int w, int h) {
        auto& ctx = Context::GetInstance();
        ctx.device.waitIdle();

        ctx.swapchain.reset();
        ctx.GetSurface();
        ctx.InitSwapchain(w, h);
        ctx.swapchain->InitFramebuffers();
    }

    ~Context();

  private:
    static Context* instance_;
    vk::SurfaceKHR surface_ = nullptr;

    GetSurfaceCallback getSurfaceCb_ = nullptr;

    Context(std::vector<const char*>& extensions, GetSurfaceCallback);

    void InitRenderProcess();
    void InitSwapchain(int windowWidth, int windowHeight);
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
} // namespace Vklib

#endif // VULKAN_LIB_CONTEXT_HPP
