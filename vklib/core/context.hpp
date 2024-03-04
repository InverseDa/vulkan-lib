#ifndef VULKAN_LIB_CONTEXT_HPP
#define VULKAN_LIB_CONTEXT_HPP

#include "vulkan/vulkan.hpp"
#include <memory>
#include <cassert>
#include <iostream>
#include <optional>

#include "tools.hpp"
#include "log/log.hpp"

namespace Vklib {
class Context final {
  public:
    static void Init();
    static void Quit();

    static Context& GetInstance() {
        assert(instance_);
        return *instance_;
    }

    ~Context();

    struct QueueFamilyIndices final {
        std::optional<uint32_t> graphicsQueue;
    };

    vk::Instance instance;
    vk::PhysicalDevice phyDevice;
    vk::Device device;
    vk::Queue graphicsQueue;
    QueueFamilyIndices queueFamilyIndices;

  private:
    static std::unique_ptr<Context> instance_;

    Context();

    void CreateInstance();
    void PickupPhysicalDevice();
    void CreateDevice();
    void GetQueues();
    void QueryQueueFamilyIndices();
};
} // namespace Vklib

#endif // VULKAN_LIB_CONTEXT_HPP
