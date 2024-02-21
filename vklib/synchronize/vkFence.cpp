#include "vkFence.h"

namespace Vulkan {

Fence::Fence(VkFenceCreateInfo& createInfo) {
    Create(createInfo);
}

Fence::Fence(VkFenceCreateFlags flags) {
    Create(flags);
}

Fence::Fence(Fence&& other) noexcept {
    MoveHandle;
}

Fence::~Fence() {
    DestroyHandleBy(vkDestroyFence);
}

ResultType Fence::Wait() const {
    VkResult result = vkWaitForFences(Context::GetInstance().GetDevice(), 1, &handle, false, UINT64_MAX);
    if (result) {
        outStream << std::format("[Fence][ERROR] Failed to wait for fence! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

ResultType Fence::Reset() const {
    VkResult result = vkResetFences(Context::GetInstance().GetDevice(), 1, &handle);
    if (result) {
        outStream << std::format("[Fence][ERROR] Failed to reset fence! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

ResultType Fence::WaitAndReset() const {
    VkResult result = Wait();
    result || (result = Reset());
    return result;
}

ResultType Fence::GetStatus() const {
    VkResult result = vkGetFenceStatus(Context::GetInstance().GetDevice(), handle);
    if (result) {
        outStream << std::format("[Fence][ERROR] Failed to get fence status! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

ResultType Fence::Create(VkFenceCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkResult result = vkCreateFence(Context::GetInstance().GetDevice(), &createInfo, nullptr, &handle);
    if (result) {
        outStream << std::format("[Fence][ERROR] Failed to create fence! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

ResultType Fence::Create(VkFenceCreateFlags flags) {
    VkFenceCreateInfo createInfo = {
        .flags = flags};
    return Create(createInfo);
}

} // namespace Vulkan