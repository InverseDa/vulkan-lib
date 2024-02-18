#ifndef VULKAN_LIB_VKRENDERPASS_H
#define VULKAN_LIB_VKRENDERPASS_H

#include "macro.h"
#include "vulkan/vulkan.h"
#include "type/vkArray.h"
#include "core/vkBase.h"
#include "core/vkFrameBuffer.h"

#include <vector>

namespace Vulkan {
class RenderPass {
  public:
    RenderPass() = default;
    RenderPass(VkRenderPassCreateInfo& renderPassCreateInfo);
    RenderPass(RenderPass&& other) noexcept;
    RenderPass& operator=(RenderPass&& other) noexcept;
    ~RenderPass();

    // Getter
    DefineHandleTypeOperator;
    DefineAddressFunction;

    void CmdBegin(VkCommandBuffer commandBuffer, VkRenderPassBeginInfo& beginInfo, VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const;
    void CmdBegin(VkCommandBuffer commandBuffer, VkFramebuffer frameBuffer, VkRect2D renderArea, ArrayRef<const VkClearValue> clearValue = {}, VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const;
    void CmdNext(VkCommandBuffer commandBuffer, VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const;
    void CmdEnd(VkCommandBuffer commandBuffer) const;

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
