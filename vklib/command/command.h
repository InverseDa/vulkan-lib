#ifndef VULKAN_LIB_COMMAND_H
#define VULKAN_LIB_COMMAND_H

#include "macro.h"
#include "type/vkResult.h"
#include "type/vkArray.h"
#include "vulkan/vulkan.h"

#include <functional>

namespace Vulkan {
class CommandBuffer {
  public:
    CommandBuffer() = default;
    CommandBuffer(CommandBuffer&& other) noexcept;
    // Getter
    DefineHandleTypeOperator;
    DefineAddressFunction;

    ResultType Begin(VkCommandBufferUsageFlags usageFlags) const;
    ResultType Begin(VkCommandBufferUsageFlags usageFlags, VkCommandBufferInheritanceInfo& inheritanceInfo) const;
    ResultType End() const;

    void SetBuffers(VkCommandBufferUsageFlags usageFlags, const std::function<void()>& func) const;
    void SetBuffers(VkCommandBufferUsageFlags usageFlags, VkCommandBufferInheritanceInfo& inheritanceInfo, const std::function<void()>& func) const;

  private:
    friend class CommandPool;
    VkCommandBuffer handle = VK_NULL_HANDLE;
};

class CommandPool {
  public:
    CommandPool() = default;
    CommandPool(VkCommandPoolCreateInfo& createInfo);
    CommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
    CommandPool(CommandPool&& other) noexcept;
    ~CommandPool();

    // Getter
    DefineHandleTypeOperator;
    DefineAddressFunction;

    ResultType AllocateBuffers(ArrayRef<VkCommandBuffer> buffers, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;
    ResultType AllocateBuffers(ArrayRef<CommandBuffer> buffers, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;
    void FreeBuffers(ArrayRef<VkCommandBuffer> buffers) const;
    void FreeBuffers(ArrayRef<CommandBuffer> buffers) const;

    ResultType Create(VkCommandPoolCreateInfo& createInfo);
    ResultType Create(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);

  private:
    VkCommandPool handle = VK_NULL_HANDLE;
};
} // namespace Vulkan

#endif // VULKAN_LIB_COMMAND_H
