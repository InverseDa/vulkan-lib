#include "renderer.hpp"
#include "vklib/core/context.hpp"
#include "vklib/mesh/vertex.hpp"

namespace Vklib {

// temporary vertex data for triangle
// TODO: REMOVE
const std::array<Vertex, 3> vertices = {
    Vertex{0.f, -.5f},
    Vertex{.5f, .5f},
    Vertex{-.5f, .5f},
};

Renderer::Renderer(int maxFlightCount) : maxFightCount_(maxFlightCount), cur_Frame_(0) {
    CreateFences();
    CreateSemaphores();
    CreateCmdBuffers();
    CreateVertexBuffer();
    BufferVertexData();
}

Renderer::~Renderer() {
    hostVertexBuffer_.reset();
    deviceVertexBuffer_.reset();
    auto& device = Context::GetInstance().device;
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

void Renderer::Render() {
    auto& ctx = Context::GetInstance();
    auto& device = ctx.device;
    if (device.waitForFences(fences_[cur_Frame_], true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess) {
        IO::ThrowError("Wait for fence failed!");
    }
    device.resetFences(fences_[cur_Frame_]);

    auto& swapchain = ctx.swapchain;
    auto resultValue = device.acquireNextImageKHR(swapchain->swapchain, std::numeric_limits<std::uint64_t>::max(), imageAvaliableSems_[cur_Frame_], nullptr);
    if (resultValue.result != vk::Result::eSuccess) {
        IO::ThrowError("Wait for image in swapchain failed!");
    }
    auto imageIndex = resultValue.value;

    auto& cmdMgr = ctx.commandMgr;
    cmdBufs_[cur_Frame_].reset();

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBufs_[cur_Frame_].begin(beginInfo);
    {
        vk::ClearValue clearValue;
        clearValue.setColor(vk::ClearColorValue(std::array<float, 4>{0.1, 0.2, 0.3, 1}));
        vk::RenderPassBeginInfo renderPassBeginInfo;
        renderPassBeginInfo
            .setRenderPass(ctx.renderProcess->renderPass)
            .setFramebuffer(swapchain->framebuffers[imageIndex])
            .setRenderArea(vk::Rect2D({}, swapchain->GetExtent()))
            .setClearValues(clearValue);
        cmdBufs_[cur_Frame_].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
        {
            cmdBufs_[cur_Frame_].bindPipeline(vk::PipelineBindPoint::eGraphics, ctx.renderProcess->graphicsPipeline);
            vk::DeviceSize offset = 0;
            cmdBufs_[cur_Frame_].bindVertexBuffers(0, deviceVertexBuffer_->buffer, offset);
            cmdBufs_[cur_Frame_].draw(3, 1, 0, 0);
        }
        cmdBufs_[cur_Frame_].endRenderPass();
    }
    cmdBufs_[cur_Frame_].end();

    vk::SubmitInfo submit;
    vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submit
        .setCommandBuffers(cmdBufs_[cur_Frame_])
        .setWaitSemaphores(imageAvaliableSems_[cur_Frame_])
        .setWaitDstStageMask(flags)
        .setSignalSemaphores(renderFinishSems_[cur_Frame_]);
    ctx.graphicsQueue.submit(submit, fences_[cur_Frame_]);

    vk::PresentInfoKHR presentInfo;
    presentInfo
        .setImageIndices(imageIndex)
        .setSwapchains(swapchain->swapchain)
        .setWaitSemaphores(renderFinishSems_[cur_Frame_]);
    if (ctx.presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
        IO::ThrowError("Present queue execute failed!");
    }

    cur_Frame_ = (cur_Frame_ + 1) % maxFightCount_;
}

void Renderer::CreateFences() {
    fences_.resize(maxFightCount_, nullptr);
    for (auto& fence : fences_) {
        vk::FenceCreateInfo fenceCreateInfo;
        fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
        fence = Context::GetInstance().device.createFence(fenceCreateInfo);
    }
}

void Renderer::CreateSemaphores() {
    auto& device = Context::GetInstance().device;
    vk::SemaphoreCreateInfo info;

    imageAvaliableSems_.resize(maxFightCount_);
    renderFinishSems_.resize(maxFightCount_);

    for (auto& sem : imageAvaliableSems_) {
        sem = device.createSemaphore(info);
    }
    for (auto& sem : renderFinishSems_) {
        sem = device.createSemaphore(info);
    }
}

void Renderer::CreateCmdBuffers() {
    cmdBufs_.resize(maxFightCount_);
    for (auto& cmdBuf : cmdBufs_) {
        cmdBuf = Context::GetInstance().commandMgr->CreateACommandBuffer();
    }
}

void Renderer::CreateVertexBuffer() {
    hostVertexBuffer_.reset(new Buffer(sizeof(vertices), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    deviceVertexBuffer_.reset(new Buffer(sizeof(vertices), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal));
}

void Renderer::BufferVertexData() {
    void* ptr = Context::GetInstance().device.mapMemory(hostVertexBuffer_->memory, 0, sizeof(vertices));
    memcpy(ptr, vertices.data(), sizeof(vertices));
    Context::GetInstance().device.unmapMemory(hostVertexBuffer_->memory);

    auto cmdBuf = Context::GetInstance().commandMgr->CreateACommandBuffer();

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuf.begin(beginInfo);
    {
        vk::BufferCopy region;
        region
            .setSize(hostVertexBuffer_->size)
            .setSrcOffset(0)
            .setDstOffset(0);
        cmdBuf.copyBuffer(hostVertexBuffer_->buffer, deviceVertexBuffer_->buffer, region);
    }
    cmdBuf.end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(cmdBuf);
    Context::GetInstance().graphicsQueue.submit(submitInfo);

    Context::GetInstance().graphicsQueue.waitIdle();

    Context::GetInstance().commandMgr->FreeCmd(cmdBuf);
}

} // namespace Vklib