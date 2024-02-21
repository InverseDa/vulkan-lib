#ifndef VULKAN_LIB_VKRENDERPASS_H
#define VULKAN_LIB_VKRENDERPASS_H

#include "macro.h"
#include "vulkan/vulkan.h"
#include "type/vkArray.h"
#include "core/context/vkContext.h"
#include "core/framebuffer/vkFrameBuffer.h"

#include <vector>
#include <functional>

namespace Vulkan {
class RenderPass {
  public:
    RenderPass() = default;
    RenderPass(VkRenderPassCreateInfo& renderPassCreateInfo);
    RenderPass(RenderPass&& other) noexcept;
    RenderPass(const RenderPass &pass);
    ~RenderPass();

    // Getter
    DefineHandleTypeOperator;
    DefineAddressFunction;

    void CmdBegin(VkCommandBuffer commandBuffer, VkRenderPassBeginInfo& beginInfo, VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const;
    void CmdBegin(VkCommandBuffer commandBuffer, VkFramebuffer frameBuffer, VkRect2D renderArea, ArrayRef<const VkClearValue> clearValue = {}, VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const;
    void CmdNext(VkCommandBuffer commandBuffer, VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const;
    void CmdEnd(VkCommandBuffer commandBuffer) const;

    void SetCommands(VkCommandBuffer commandBuffer, VkRenderPassBeginInfo& beginInfo, const std::function<void()>& commands = [] {}, VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const;
    void SetCommands(VkCommandBuffer commandBuffer, VkFramebuffer frameBuffer, VkRect2D renderArea, ArrayRef<const VkClearValue> clearValue = {}, const std::function<void()>& commands = [] {}, VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const;

    ResultType Create(VkRenderPassCreateInfo& renderPassCreateInfo);

  private:
    VkRenderPass handle = VK_NULL_HANDLE;
};

struct RenderPassWithFrameBuffers {
    RenderPass renderPass;
    std::vector<FrameBuffer> frameBuffers;
};

const RenderPassWithFrameBuffers& CreateRenderPassWithFrameBuffersScreen();

} // namespace Vulkan

#endif // VULKAN_LIB_VKRENDERPASS_H