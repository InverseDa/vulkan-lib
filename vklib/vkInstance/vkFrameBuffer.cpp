#include "vkInstance/vkFrameBuffer.h"

namespace Vulkan {
FrameBuffer::FrameBuffer(VkFramebufferCreateInfo& createInfo) {
    Create(createInfo);
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept {
    MoveHandle;
}

FrameBuffer::~FrameBuffer() {
    DestroyHandleBy(vkDestroyFramebuffer);
}

ResultType FrameBuffer::Create(VkFramebufferCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    VkResult result = vkCreateFramebuffer(GraphicsBase::GetInstance().GetDevice(), &createInfo, nullptr, &handle);
    if (result) {
        std::cout << std::format("[FrameBuffer][ERROR] Failed to create a framebuffer! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

} // namespace Vulkan