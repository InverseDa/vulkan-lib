#include "renderer.hpp"
#include "core/context.hpp"
#include "math/float2.hpp"
#include "math/uniform.hpp"
#include "mesh/vertex.hpp"

namespace Vklib {

Renderer::Renderer(int maxFlightCount) : maxFightCount_(maxFlightCount), cur_Frame_(0) {
    CreateFences();
    CreateSemaphores();
    CreateCmdBuffers();
    CreateBuffers();
    CreateUniformBuffers(maxFlightCount);
    descriptorSets_ = DescriptorSetMgr::GetInstance().AllocateBufferSets(maxFlightCount);
    UpdateDescriptorSets();
    InitMats();
    CreateWhiteTexture();
}

Renderer::~Renderer() {
    auto& device = Context::GetInstance().device;
    device.destroySampler(sampler);
    rectVerticesBuffer_.reset();
    rectIndicesBuffer_.reset();
    uniformBuffers_.clear();
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

void Renderer::InitMats() {
    viewMat_ = mat4::CreateIdentity();
    projectionMat_ = mat4::CreateIdentity();
}

void Renderer::DrawRect(const Rect& rect, Texture& texture) {
    auto& ctx = Context::GetInstance();
    auto& device = ctx.device;
    auto& cmd = cmdBufs_[cur_Frame_];
    vk::DeviceSize offset = 0;

    BufferRectData();

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, ctx.renderProcess->graphicsPipelineTriangleTopology);
    cmd.bindVertexBuffers(0, rectVerticesBuffer_->buffer, offset);
    cmd.bindIndexBuffer(rectIndicesBuffer_->buffer, 0, vk::IndexType::eUint32);

    auto& layout = Context::GetInstance().renderProcess->layout;
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                           layout,
                           0,
                           {descriptorSets_[cur_Frame_].set, texture.set.set},
                           {});
    auto model = mat4::CreateTranslate(rect.position) * mat4::CreateScale(rect.size);
    cmd.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(mat4), model.GetData());
    cmd.pushConstants(layout, vk::ShaderStageFlagBits::eFragment, sizeof(mat4), sizeof(Color), &drawColor_);
    cmd.drawIndexed(6, 1, 0, 0, 0);
}

void Renderer::DrawLine(const float2& p1, const float2& p2) {
    auto& ctx = Context::GetInstance();
    auto& device = ctx.device;
    auto& cmd = cmdBufs_[cur_Frame_];
    vk::DeviceSize offset = 0;

    BufferLineData(p1, p2);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, ctx.renderProcess->graphicsPipelineLineTopology);
    cmd.bindVertexBuffers(0, lineVerticesBuffer_->buffer, offset);

    auto& layout = Context::GetInstance().renderProcess->layout;
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                           layout,
                           0,
                           {descriptorSets_[cur_Frame_].set, whiteTexture->set.set},
                           {});
    auto model = mat4::CreateIdentity();
    cmd.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(mat4), model.GetData());
    cmd.pushConstants(layout, vk::ShaderStageFlagBits::eFragment, sizeof(mat4), sizeof(Color), &drawColor_);
    cmd.draw(2, 1, 0, 0);
}

void Renderer::StartRender() {
    auto& ctx = Context::GetInstance();
    auto& device = ctx.device;
    if (device.waitForFences(fences_[cur_Frame_], true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess) {
        IO::ThrowError("Wait for fence failed!");
    }
    device.resetFences(fences_[cur_Frame_]);

    auto& swapchain = ctx.swapchain;
    auto resultValue = device.acquireNextImageKHR(swapchain->swapchain,
                                                  std::numeric_limits<std::uint64_t>::max(),
                                                  imageAvaliableSems_[cur_Frame_],
                                                  nullptr);
    if (resultValue.result != vk::Result::eSuccess) {
        IO::ThrowError("Wait for image in swapchain failed!");
    }
    imageIndex_ = resultValue.value;

    auto& cmdMgr = ctx.commandMgr;
    auto& cmd = cmdBufs_[cur_Frame_];
    cmd.reset();

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmd.begin(beginInfo);

    vk::ClearValue clearValue;
    clearValue.setColor(vk::ClearColorValue(std::array<float, 4>{0.2, 0.3, 0.3, 1}));
    vk::RenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo
        .setRenderPass(ctx.renderProcess->renderPass)
        .setFramebuffer(swapchain->framebuffers[imageIndex_])
        .setRenderArea(vk::Rect2D({}, swapchain->GetExtent()))
        .setClearValues(clearValue);

    cmd.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

void Renderer::EndRender() {
    auto& ctx = Context::GetInstance();
    auto& swapchain = ctx.swapchain;
    auto& cmd = cmdBufs_[cur_Frame_];

    cmd.endRenderPass();
    cmd.end();
    vk::SubmitInfo submit;
    vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submit
        .setCommandBuffers(cmd)
        .setWaitSemaphores(imageAvaliableSems_[cur_Frame_])
        .setWaitDstStageMask(flags)
        .setSignalSemaphores(renderFinishSems_[cur_Frame_]);
    ctx.graphicsQueue.submit(submit, fences_[cur_Frame_]);

    vk::PresentInfoKHR presentInfo;
    presentInfo
        .setImageIndices(imageIndex_)
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

void Renderer::CreateBuffers() {
    auto& device = Context::GetInstance().device;
    rectVerticesBuffer_.reset(new Buffer(vk::BufferUsageFlagBits::eVertexBuffer,
                                         sizeof(Vertex2) * 4,
                                         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    rectIndicesBuffer_.reset(new Buffer(vk::BufferUsageFlagBits::eIndexBuffer,
                                        sizeof(uint32_t) * 6,
                                        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    lineVerticesBuffer_.reset(new Buffer(vk::BufferUsageFlagBits::eVertexBuffer,
                                         sizeof(Vertex2) * 2,
                                         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
}

void Renderer::CreateUniformBuffers(int flightCount) {
    uniformBuffers_.resize(flightCount);
    size_t size = sizeof(mat4) * 2;
    for (auto& buffer : uniformBuffers_) {
        buffer.reset(new Buffer(vk::BufferUsageFlagBits::eTransferSrc,
                                size,
                                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    }

    deviceUniformBuffers_.resize(flightCount);
    for (auto& buffer : deviceUniformBuffers_) {
        buffer.reset(new Buffer(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer,
                                size,
                                vk::MemoryPropertyFlagBits::eDeviceLocal));
    }
}

void Renderer::TransBuffer2Device(Buffer& src, Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset) {
    Context::GetInstance().commandMgr->ExecuteCmd(Context::GetInstance().graphicsQueue, [&](vk::CommandBuffer& cmdBuf) {
        vk::BufferCopy region;
        region.setSrcOffset(srcOffset)
            .setDstOffset(dstOffset)
            .setSize(size);
        cmdBuf.copyBuffer(src.buffer, dst.buffer, region);
    });
}

void Renderer::BufferRectData() {
    BufferRectVertexData();
    BufferRectIndicesData();
}

void Renderer::BufferRectVertexData() {
    Vertex2 vertices[] = {
        {{-0.5, -0.5}, {0, 0}},
        {{0.5, -0.5}, {1, 0}},
        {{0.5, 0.5}, {1, 1}},
        {{-0.5, 0.5}, {0, 1}},
    };
    memcpy(rectVerticesBuffer_->map, vertices, sizeof(vertices));
}

void Renderer::BufferRectIndicesData() {
    std::uint32_t indices[] = {
        0,
        1,
        3,
        1,
        2,
        3,
    };
    memcpy(rectIndicesBuffer_->map, indices, sizeof(indices));
}

void Renderer::BufferLineData(const float2& p1, const float2& p2) {
    Vertex2 vertices[] = {
        {p1, {0, 0}},
        {p2, {0, 0}},
    };
    memcpy(lineVerticesBuffer_->map, vertices, sizeof(vertices));
}

void Renderer::BufferMVPData() {
    for (int i = 0; i < uniformBuffers_.size(); i++) {
        auto& buffer = uniformBuffers_[i];
        memcpy(buffer->map, (void*)&projectionMat_, sizeof(mat4));
        memcpy(((float*)buffer->map + 4 * 4), (void*)&viewMat_, sizeof(mat4));
        TransBuffer2Device(*buffer, *deviceUniformBuffers_[i], buffer->size, 0, 0);
    }
}

void Renderer::SetDrawColor(const Color& color) {
    drawColor_ = color;
}

void Renderer::UpdateDescriptorSets() {
    for (int i = 0; i < descriptorSets_.size(); i++) {
        // bind MVP buffer
        vk::DescriptorBufferInfo bufferInfo1;
        bufferInfo1
            .setBuffer(deviceUniformBuffers_[i]->buffer)
            .setOffset(0)
            .setRange(sizeof(mat4) * 2);

        std::vector<vk::WriteDescriptorSet> writerInfos(1);
        writerInfos[0]
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDstSet(descriptorSets_[i].set)
            .setDstBinding(0)
            .setDstArrayElement(0)
            .setDescriptorCount(1)
            .setBufferInfo(bufferInfo1);

        Context::GetInstance().device.updateDescriptorSets(writerInfos, {});
    }
}

void Renderer::SetProjectionMatrix(int left, int right, int top, int bottom, int near, int far) {
    projectionMat_ = mat4::CreateOrtho(left, right, top, bottom, near, far);
    BufferMVPData();
}

void Renderer::CreateWhiteTexture() {
    unsigned char data[] = {0xFF, 0xFF, 0xFF, 0xFF};
    whiteTexture = TextureMgr::GetInstance().Create((void*)data, 1, 1);
}

} // namespace Vklib