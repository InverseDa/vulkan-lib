#include "semaphore.h"

namespace Vulkan {

Semaphore::Semaphore(VkSemaphoreCreateInfo& createInfo) {
    Create(createInfo);
}

Semaphore::Semaphore() {
    Create();
}

Semaphore::Semaphore(Semaphore&& other) noexcept {
    MoveHandle;
}

Semaphore::~Semaphore() {
    DestroyHandleBy(vkDestroySemaphore);
}

ResultType Semaphore::Create() {
    VkSemaphoreCreateInfo createInfo = {};
    return Create(createInfo);
}

ResultType Semaphore::Create(VkSemaphoreCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkResult result = vkCreateSemaphore(GraphicsBase::GetInstance().GetDevice(), &createInfo, nullptr, &handle);
    if (result) {
        outStream << std::format("Failed to create semaphore! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

} // namespace Vulkan