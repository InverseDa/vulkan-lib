#ifndef VULKAN_LIB_VKFRAMEBUFFER_H
#define VULKAN_LIB_VKFRAMEBUFFER_H

#include "macro.h"
#include "vulkan/vulkan.h"

#include <format>

namespace Vulkan {
class FrameBuffer {
  public:
    FrameBuffer() = default;
    FrameBuffer(VkFramebufferCreateInfo& createInfo);
    FrameBuffer(FrameBuffer&& other) noexcept;
    FrameBuffer(const FrameBuffer& frameBuffer);
    ~FrameBuffer();

    // Getter
    DefineHandleTypeOperator;
    DefineAddressFunction;

    ResultType Create(VkFramebufferCreateInfo& createInfo);

  private:
    VkFramebuffer handle;
};
} // namespace Vulkan

#endif // VULKAN_LIB_VKFRAMEBUFFER_H
