#ifndef VULKAN_LIB_BUFFER_H
#define VULKAN_LIB_BUFFER_H

#include "macro.h"
#include "command/pool.h"
#include "type/vkResult.h"
#include "vulkan/vulkan.h"

namespace Vulkan {
class CommandBuffer {
  public:
    CommandBuffer() = default;
    CommandBuffer(CommandBuffer&& other) noexcept;
    // Getter
    DefineHandleTypeOperator;
    DefineAddressFunction;

    ResultType Begin(VkCommandBufferUsageFlags usageFlags, VkCommandBufferInheritanceInfo& inheritanceInfo) const;
    ResultType Begin(VkCommandBufferUsageFlags usageFlags) const;
    ResultType End() const;

  private:
    friend class CommandPool;
    VkCommandBuffer handle = VK_NULL_HANDLE;
};
} // namespace Vulkan

#endif // VULKAN_LIB_BUFFER_H
