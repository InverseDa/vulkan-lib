#ifndef VULKAN_LIB_VKFENCE_H
#define VULKAN_LIB_VKFENCE_H

#include "vulkan/vulkan.h"
#include "macro.h"
#include "type/vkResult.h"
#include "core/ostream.h"

namespace Vulkan {
class Fence {
  public:
    Fence() = default;
    Fence(VkFenceCreateInfo& createInfo);
    Fence(VkFenceCreateFlags flags = 0);
    Fence(Fence&& other) noexcept;
    ~Fence();

    // Getter
    DefineHandleTypeOperator;
    DefineAddressFunction;

    ResultType Wait() const;
    ResultType Reset() const;
    ResultType WaitAndReset() const;
    ResultType GetStatus() const;

    ResultType Create(VkFenceCreateInfo& createInfo);
    ResultType Create(VkFenceCreateFlags flags = 0);

  private:
    VkFence handle = VK_NULL_HANDLE;
};
} // namespace Vulkan

#endif // VULKAN_LIB_VKFENCE_H