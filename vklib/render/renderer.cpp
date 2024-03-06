#include "renderer.hpp"
#include "vklib/core/context.hpp"

namespace Vklib {
Renderer::Renderer() {
    InitCmdPool();
    AllocateCmdBuf();
    CreateSems();
    CreateFence();
}

Renderer::~Renderer() {
    auto& device = Context::GetInstance().device;
    device.freeCommandBuffers(cmdPool_, cmdBuf_);
    device.destroyCommandPool(cmdPool_);
    device.destroySemaphore(imageAvailable_);
    device.destroySemaphore(imageDrawFinish_);
    device.destroyFence(cmdAvailableFence_);
}

void Renderer::InitCmdPool() {
    vk::CommandPoolCreateInfo createInfo;
    createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    cmdPool_ = Context::GetInstance().device.createCommandPool(createInfo);
}

void Renderer::AllocateCmdBuf() {
    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo
        .setCommandPool(cmdPool_)
        .setCommandBufferCount(1)
        .setLevel(vk::CommandBufferLevel::ePrimary);

    cmdBuf_ = Context::GetInstance().device.allocateCommandBuffers(allocateInfo)[0];
}

void Renderer::CreateSems() {
    vk::SemaphoreCreateInfo createInfo;

    imageAvailable_ = Context::GetInstance().device.createSemaphore(createInfo);
    imageDrawFinish_ = Context::GetInstance().device.createSemaphore(createInfo);
}

void Renderer::CreateFence() {
    vk::FenceCreateInfo createInfo;

    cmdAvailableFence_ = Context::GetInstance().device.createFence(createInfo);
}

void Renderer::Render() {
    auto& device = Context::GetInstance().device;
    auto& renderProcess = Context::GetInstance().renderProcess;
    auto& swapchain = Context::GetInstance().swapchain;

    auto result = device.acquireNextImageKHR(Context::GetInstance().swapchain->swapchain, std::numeric_limits<uint64_t>::max(), imageAvailable_);
    if (result.result != vk::Result::eSuccess) {
        IO::PrintLog(LOG_LEVEL_WARNING, "Acquire next image failed!");
    }

    auto imageIdx = result.value;

    cmdBuf_.reset();

    vk::CommandBufferBeginInfo begin;
    begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuf_.begin(begin);
    {
        vk::RenderPassBeginInfo renderPassBeginInfo;
        vk::Rect2D area;
        vk::ClearValue clearValue;
        clearValue.color = vk::ClearColorValue(std::array<float, 4>{0.1f, 0.2f, 0.3f, 1.0f});
        area.setOffset({0, 0})
            .setExtent(swapchain->info.imageExtent);
        renderPassBeginInfo
            .setRenderPass(renderProcess->renderPass)
            .setRenderArea(area)
            .setFramebuffer(swapchain->framebuffers[imageIdx])
            .setClearValues(clearValue);

        cmdBuf_.beginRenderPass(renderPassBeginInfo, {});
        {
            cmdBuf_.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->pipeline);
            cmdBuf_.draw(3, 1, 0, 0);
        }
        cmdBuf_.endRenderPass();
    }
    cmdBuf_.end();

    vk::SubmitInfo submit;
    submit
        .setCommandBuffers(cmdBuf_)
        .setWaitSemaphores(imageAvailable_)
        .setSignalSemaphores(imageDrawFinish_);
    Context::GetInstance().graphicsQueue.submit(submit, cmdAvailableFence_);

    vk::PresentInfoKHR present;
    present
        .setImageIndices(imageIdx)
        .setSwapchains(swapchain->swapchain)
        .setWaitSemaphores(imageDrawFinish_);
    if (Context::GetInstance().presentQueue.presentKHR(present) != vk::Result::eSuccess) {
        IO::PrintLog(LOG_LEVEL_WARNING, "Image present failed");
    }

    if (device.waitForFences(cmdAvailableFence_, true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
        IO::PrintLog(LOG_LEVEL_WARNING, "Wait for fence failed");
    }

    Context::GetInstance().device.resetFences(cmdAvailableFence_);
}

} // namespace Vklib