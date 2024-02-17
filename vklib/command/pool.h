#ifndef VULKAN_LIB_POOL_H
#define VULKAN_LIB_POOL_H

#include "macro.h"
#include "buffer.h"
#include "command/pool.h"
#include "type/vkResult.h"
#include "type/vkArray.h"
#include "vulkan/vulkan.h"

namespace Vulkan {
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

#endif // VULKAN_LIB_POOL_H
