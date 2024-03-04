#ifndef VULKAN_LIB_CONTEXT_HPP
#define VULKAN_LIB_CONTEXT_HPP

#include "vulkan/vulkan.hpp"
#include <memory>
#include <cassert>
#include <iostream>
#include <optional>

#include "log/log.hpp"
#include "vklib/tools.hpp"
#include "vklib/swapchain/swapchain.hpp"

namespace Vklib {
class Context final {
  public:
    static void Init(const std::vector<const char*>& extensions, CreateSurfaceFunc func);
    static void Quit();

    static Context& GetInstance() {
        assert(instance_);
        return *instance_;
    }

    ~Context();

    struct QueueFamilyIndices final {
        std::optional<uint32_t> graphicsQueue;
        std::optional<uint32_t> presentQueue;

        operator bool() const {
            return graphicsQueue.has_value() && presentQueue.has_value();
        }
    };

    vk::Instance instance;
    vk::PhysicalDevice phyDevice;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    vk::SurfaceKHR surface;
    std::unique_ptr<Swapchain> swapchain;
    QueueFamilyIndices queueFamilyIndices;

    void InitSwapchain(int w, int h) {
        swapchain = std::make_unique<Swapchain>(w, h);
    }

    void DestroySwapchain() {
        swapchain.reset();
    }

  private:
    static std::unique_ptr<Context> instance_;

    Context(const std::vector<const char*>& extensions, CreateSurfaceFunc func);

    void CreateInstance(const std::vector<const char*>& extensions);
    void PickupPhysicalDevice();
    void CreateDevice();
    void GetQueues();

    void QueryQueueFamilyIndices();
};
} // namespace Vklib

#endif // VULKAN_LIB_CONTEXT_HPP
