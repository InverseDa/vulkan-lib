#ifndef VULKAN_LIB_FENCE_H
#define VULKAN_LIB_FENCE_H

#include "vulkan/vulkan.h"

namespace Vulkan {
class Fence {
  public:
    Fence() = default;
    Fence(VkFenceCreateInfo& createInfo);
    Fence(VkFenceCreateFlags flags = 0);
    Fence(Fence&&) noexcept;
    ~Fence();

    Fence& operator=(const Fence&) = delete;
    Fence& operator=(Fence&&) = delete;

    void CreateFence();
    void DestroyFence();
    void ResetFence();
    void WaitFence();
    void WaitFenceWithTimeout(uint64_t timeout);
    void GetFenceStatus();
    void GetFenceStatusWithTimeout(uint64_t timeout);

  private:
    VkFence fence;
};
} // namespace Vulkan

#endif // VULKAN_LIB_FENCE_H
