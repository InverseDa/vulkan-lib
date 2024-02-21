#include "command.h"

namespace Vulkan {
CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept {
    MoveHandle;
}

ResultType CommandBuffer::Begin(VkCommandBufferUsageFlags usageFlags, VkCommandBufferInheritanceInfo& inheritanceInfo) const {
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = usageFlags,
        .pInheritanceInfo = &inheritanceInfo
    };
    VkResult result = vkBeginCommandBuffer(handle, &beginInfo);
    if (result) {
        outStream << std::format("[CommandBuffer][ERROR] Failed to begin a command buffer! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

ResultType CommandBuffer::Begin(VkCommandBufferUsageFlags usageFlags) const {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = usageFlags
    };
    VkResult result = vkBeginCommandBuffer(handle, &beginInfo);
    if (result) {
        outStream << std::format("[CommandBuffer][ERROR] Failed to begin a command buffer! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

ResultType CommandBuffer::End() const {
    VkResult result = vkEndCommandBuffer(handle);
    if (result) {
        outStream << std::format("[CommandBuffer][ERROR] Failed to end a command buffer! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

void CommandBuffer::Record(VkCommandBufferUsageFlags usageFlags, const std::function<void()>& func) const {
    Begin(usageFlags);
    func();
    End();
}

void CommandBuffer::Record(VkCommandBufferUsageFlags usageFlags, VkCommandBufferInheritanceInfo& inheritanceInfo, const std::function<void()>& func) const {
    Begin(usageFlags, inheritanceInfo);
    func();
    End();
}

} // namespace Vulkan