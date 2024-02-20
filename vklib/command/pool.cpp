#include "command.h"

namespace Vulkan {
CommandPool::CommandPool(VkCommandPoolCreateInfo& createInfo) {
    Create(createInfo);
}

CommandPool::CommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags) {
    Create(queueFamilyIndex, flags);
}

CommandPool::CommandPool(CommandPool&& other) noexcept {
    MoveHandle;
}

CommandPool::~CommandPool() {
    DestroyHandleBy(vkDestroyCommandPool);
}

ResultType CommandPool::AllocateBuffers(ArrayRef<VkCommandBuffer> buffers, VkCommandBufferLevel level) const {
    VkCommandBufferAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = handle,
        .level = level,
        .commandBufferCount = uint32_t(buffers.Count())
    };
    VkResult result = vkAllocateCommandBuffers(Context::GetInstance().GetDevice(), &allocateInfo, buffers.Pointer());
    if (result) {
        outStream << std::format("[CommandPool][ERROR] Failed to allocate command buffers! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

ResultType CommandPool::AllocateBuffers(ArrayRef<CommandBuffer> buffers, VkCommandBufferLevel level) const {
    return AllocateBuffers({&buffers[0].handle, buffers.Count()}, level);
}

void CommandPool::FreeBuffers(ArrayRef<VkCommandBuffer> buffers) const {
    Vulkan::Context::GetInstance().WaitDeviceIdle();
    vkFreeCommandBuffers(Context::GetInstance().GetDevice(), handle, buffers.Count(), buffers.Pointer());
    memset(buffers.Pointer(), 0, buffers.Count() * sizeof(VkCommandBuffer));
}

void CommandPool::FreeBuffers(ArrayRef<CommandBuffer> buffers) const {
    FreeBuffers({&buffers[0].handle, buffers.Count()});
}

ResultType CommandPool::Create(VkCommandPoolCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    VkResult result = vkCreateCommandPool(Context::GetInstance().GetDevice(), &createInfo, nullptr, &handle);
    if (result) {
        outStream << std::format("[CommandPool][ERROR] Failed to create a command pool! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

ResultType CommandPool::Create(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags) {
    VkCommandPoolCreateInfo createInfo = {
        .flags = flags,
        .queueFamilyIndex = queueFamilyIndex,
    };
    return Create(createInfo);
}

} // namespace Vulkan