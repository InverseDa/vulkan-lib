#include "renderer.hpp"
#include "core/context.hpp"

namespace ida {
IdaRenderer::IdaRenderer() : curFrame_(0) {
    CreateFences();
    CreateSemaphores();
    CreateCmdBuffers();
    CreateBuffers();
}

IdaRenderer::~IdaRenderer() {
    auto& device = Context::GetInstance().device;
    device.destroySampler(sampler);
    for (auto& sem : imageAvaliableSems_) {
        device.destroySemaphore(sem);
    }
    for (auto& sem : renderFinishSems_) {
        device.destroySemaphore(sem);
    }
    for (auto& fence : fences_) {
        device.destroyFence(fence);
    }
}

void IdaRenderer::StartRender() {
    auto& ctx = Context::GetInstance();
    auto& device = ctx.device;
    if (device.waitForFences(fences_[curFrame_], true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess) {
        IO::ThrowError("Wait for fence failed!");
    }
    device.resetFences(fences_[curFrame_]);

    auto& swapchain = ctx.swapChain;
    auto resultValue = device.acquireNextImageKHR(swapchain->swapchain,
                                                  std::numeric_limits<std::uint64_t>::max(),
                                                  imageAvaliableSems_[curFrame_],
                                                  nullptr);
    if (resultValue.result != vk::Result::eSuccess) {
        IO::ThrowError("Wait for image in swapchain failed!");
    }
    curImageIndex_ = resultValue.value;

    auto& cmdMgr = ctx.commandMgr;
    auto& cmd = cmdBufs_[curFrame_];
    cmd.reset();

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmd.begin(beginInfo);

    vk::ClearValue clearValue;
    clearValue.setColor(vk::ClearColorValue(std::array<float, 4>{0.2, 0.3, 0.3, 1}));
    vk::RenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo
        .setRenderPass(ctx.renderProcess->renderPass)
        .setFramebuffer(swapchain->framebuffers[curImageIndex_])
        .setRenderArea(vk::Rect2D({}, swapchain->GetExtent()))
        .setClearValues(clearValue);

    cmd.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

void IdaRenderer::EndRender() {
    auto& ctx = Context::GetInstance();
    auto& swapchain = ctx.swapChain;
    auto& cmd = cmdBufs_[curFrame_];

    cmd.endRenderPass();
    cmd.end();
    vk::SubmitInfo submit;
    vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submit
        .setCommandBuffers(cmd)
        .setWaitSemaphores(imageAvaliableSems_[curFrame_])
        .setWaitDstStageMask(flags)
        .setSignalSemaphores(renderFinishSems_[curFrame_]);
    ctx.graphicsQueue.submit(submit, fences_[curFrame_]);

    vk::PresentInfoKHR presentInfo;
    presentInfo
        .setImageIndices(curImageIndex_)
        .setSwapchains(swapchain->swapchain)
        .setWaitSemaphores(renderFinishSems_[curFrame_]);
    if (ctx.presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
        IO::ThrowError("Present queue execute failed!");
    }

    curFrame_ = (curFrame_ + 1) % IdaSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void IdaRenderer::CreateFences() {
    fences_.resize(IdaSwapChain::MAX_FRAMES_IN_FLIGHT, nullptr);
    for (auto& fence : fences_) {
        vk::FenceCreateInfo fenceCreateInfo;
        fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
        fence = Context::GetInstance().device.createFence(fenceCreateInfo);
    }
}

void IdaRenderer::CreateSemaphores() {
    auto& device = Context::GetInstance().device;
    vk::SemaphoreCreateInfo info;

    imageAvaliableSems_.resize(IdaSwapChain::MAX_FRAMES_IN_FLIGHT);
    renderFinishSems_.resize(IdaSwapChain::MAX_FRAMES_IN_FLIGHT);

    for (auto& sem : imageAvaliableSems_) {
        sem = device.createSemaphore(info);
    }
    for (auto& sem : renderFinishSems_) {
        sem = device.createSemaphore(info);
    }
}

void IdaRenderer::CreateCmdBuffers() {
    cmdBufs_.resize(IdaSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (auto& cmdBuf : cmdBufs_) {
        cmdBuf = Context::GetInstance().commandMgr->CreateACommandBuffer();
    }
}

void IdaRenderer::TransBuffer2Device(IdaBuffer& src, IdaBuffer& dst, size_t size, size_t srcOffset, size_t dstOffset) {
    Context::GetInstance().commandMgr->ExecuteCmd(Context::GetInstance().graphicsQueue, [&](vk::CommandBuffer& cmdBuf) {
        vk::BufferCopy region;
        region.setSrcOffset(srcOffset)
            .setDstOffset(dstOffset)
            .setSize(size);
        cmdBuf.copyBuffer(src.buffer, dst.buffer, region);
    });
}

} // namespace Vklib