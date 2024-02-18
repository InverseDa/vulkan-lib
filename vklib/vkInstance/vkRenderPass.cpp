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

const RenderPassWithFrameBuffers& CreateRenderPassWithFrameBuffersScreen() {
    static RenderPassWithFrameBuffers renderPassWithFrameBuffers;
    if (renderPassWithFrameBuffers.renderPass) {
        outStream << "Render pass already created" << std::endl;
    } else {
        VkAttachmentDescription attachmentDescription = {
            .format = GraphicsBase::GetInstance().GetSwapChainCreateInfo().imageFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };
        VkAttachmentReference attachmentReference = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };
        VkSubpassDescription subpassDescription = {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &attachmentReference,
        };
        VkSubpassDependency subpassDependency = {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        };
        VkRenderPassCreateInfo renderPassCreateInfo = {
            .attachmentCount = 1,
            .pAttachments = &attachmentDescription,
            .subpassCount = 1,
            .pSubpasses = &subpassDescription,
            .dependencyCount = 1,
            .pDependencies = &subpassDependency,
        };
        renderPassWithFrameBuffers.renderPass.Create(renderPassCreateInfo);
        auto CreateFrameBuffers = [] {
            renderPassWithFrameBuffers.frameBuffers.resize(GraphicsBase::GetInstance().GetSwapChainImageCount());
            // TODO: may use window size
            VkFramebufferCreateInfo framebufferCreateInfo = {
                .renderPass = renderPassWithFrameBuffers.renderPass,
                .attachmentCount = 1,
                .width = GraphicsBase::GetInstance().GetSwapChainCreateInfo().imageExtent.width,
                .height = GraphicsBase::GetInstance().GetSwapChainCreateInfo().imageExtent.height,
                .layers = 1,
            };
            for (size_t i = 0; i < GraphicsBase::GetInstance().GetSwapChainImageCount(); i++) {
                VkImageView attachment = GraphicsBase::GetInstance().GetSwapChainImageView(i);
                framebufferCreateInfo.pAttachments = &attachment;
                renderPassWithFrameBuffers.frameBuffers[i].Create(framebufferCreateInfo);
            }
        };
        auto DestroyFrameBuffers = [] {
            renderPassWithFrameBuffers.frameBuffers.clear();
        };
        GraphicsBase::GetInstance().AddCallbackCreateSwapChain(CreateFrameBuffers);
        GraphicsBase::GetInstance().AddCallbackDestroySwapChain(DestroyFrameBuffers);
        CreateFrameBuffers();
    }
    return renderPassWithFrameBuffers;
}

} // namespace Vulkan