#include "vkInstance/vkRenderPass.h"

namespace Vulkan {
RenderPass::RenderPass(VkRenderPassCreateInfo& renderPassCreateInfo) {
    Create(renderPassCreateInfo);
}

RenderPass::RenderPass(RenderPass&& other) noexcept {
    MoveHandle;
}

RenderPass& RenderPass::operator=(RenderPass&& other) noexcept {
    MoveHandle;
    return *this;
}

RenderPass::~RenderPass() {
    DestroyHandleBy(vkDestroyRenderPass);
}

void RenderPass::CmdBegin(VkCommandBuffer commandBuffer, VkRenderPassBeginInfo& beginInfo, VkSubpassContents subpassContents) const {
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = handle;
    vkCmdBeginRenderPass(commandBuffer, &beginInfo, subpassContents);
}

void RenderPass::CmdBegin(VkCommandBuffer commandBuffer, VkFramebuffer frameBuffer, VkRect2D renderArea, ArrayRef<const VkClearValue> clearValue, VkSubpassContents subpassContents) const {
    VkRenderPassBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = handle,
        .framebuffer = frameBuffer,
        .renderArea = renderArea,
        .clearValueCount = uint32_t(clearValue.Count()),
        .pClearValues = clearValue.Pointer(),
    };
    vkCmdBeginRenderPass(commandBuffer, &beginInfo, subpassContents);
}

void RenderPass::CmdNext(VkCommandBuffer commandBuffer, VkSubpassContents subpassContents) const {
    vkCmdNextSubpass(commandBuffer, subpassContents);
}

void RenderPass::CmdEnd(VkCommandBuffer commandBuffer) const {
    vkCmdEndRenderPass(commandBuffer);
}

ResultType RenderPass::Create(VkRenderPassCreateInfo& renderPassCreateInfo) {
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    VkResult result = vkCreateRenderPass(GraphicsBase::GetInstance().GetDevice(), &renderPassCreateInfo, nullptr, &handle);
    if (result) {
        std::cout << std::format("[RenderPass][ERROR] Failed to create a render pass! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

} // namespace Vulkan