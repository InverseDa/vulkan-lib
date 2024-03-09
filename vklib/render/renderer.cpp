#include "renderer.hpp"
#include "vklib/core/context.hpp"
#include "vklib/math/vec2.hpp"
#include "vklib/math/uniform.hpp"
#include "vklib/mesh/vertex.hpp"

namespace Vklib {

Renderer::Renderer(int maxFlightCount) : maxFightCount_(maxFlightCount), cur_Frame_(0) {
    CreateFences();
    CreateSemaphores();
    CreateCmdBuffers();
    CreateBuffers();
    CreateUniformBuffers(maxFlightCount);
    BufferData();
    descriptorSets_ = DescriptorSetMgr::GetInstance().AllocateBufferSets(maxFlightCount);
    UpdateDescriptorSets();
    InitMats();
}

Renderer::~Renderer() {
    auto& device = Context::GetInstance().device;
    device.destroySampler(sampler);
    verticesBuffer_.reset();
    indicesBuffer_.reset();
    uniformBuffers_.clear();
    colorBuffers_.clear();
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
    viewMat_ = Mat4::CreateIdentity();
    projectionMat_ = Mat4::CreateIdentity();
}

void Renderer::Draw(const Rect& rect, Texture& texture) {
    auto& ctx = Context::GetInstance();
    auto& device = ctx.device;
    auto& cmd = cmdBufs_[cur_Frame_];

    vk::DeviceSize offset = 0;
    cmd.bindVertexBuffers(0, verticesBuffer_->buffer, offset);
    cmd.bindIndexBuffer(indicesBuffer_->buffer, 0, vk::IndexType::eUint32);
    auto& layout = Context::GetInstance().renderProcess->layout;
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                           layout,
                           0,
                           { descriptorSets_[cur_Frame_].set, texture.set.set},
                           {});
    auto model = Mat4::CreateTranslate(rect.position) * Mat4::CreateScale(rect.size);
    cmd.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(Mat4), &model);
    cmd.drawIndexed(6, 1, 0, 0, 0);
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

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, ctx.renderProcess->graphicsPipeline);

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
    verticesBuffer_.reset(new Buffer(vk::BufferUsageFlagBits::eVertexBuffer,
                                     sizeof(Vertex) * 4,
                                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    indicesBuffer_.reset(new Buffer(vk::BufferUsageFlagBits::eIndexBuffer,
                                    sizeof(uint32_t) * 6,
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
}

void Renderer::CreateUniformBuffers(int flightCount) {
    uniformBuffers_.resize(flightCount);
    size_t size = sizeof(Mat4) * 2;
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

    colorBuffers_.resize(flightCount);
    size = sizeof(Color);
    for (auto& buffer : colorBuffers_) {
        buffer.reset(new Buffer(vk::BufferUsageFlagBits::eTransferSrc,
                                size,
                                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    }

    deviceColorBuffers_.resize(flightCount);
    for (auto& buffer : deviceColorBuffers_) {
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

void Renderer::BufferData() {
    BufferVertexData();
    BufferIndicesData();
}

void Renderer::BufferVertexData() {
    Vertex vertices[] = {
        {{-0.5, -0.5}, {0, 0}},
        {{0.5, -0.5}, {1, 0}},
        {{0.5, 0.5}, {1, 1}},
        {{-0.5, 0.5}, {0, 1}},
    };
    memcpy(verticesBuffer_->map, vertices, sizeof(vertices));
}

void Renderer::BufferIndicesData() {
    std::uint32_t indices[] = {
        0,
        1,
        3,
        1,
        2,
        3,
    };
    memcpy(indicesBuffer_->map, indices, sizeof(indices));
}

void Renderer::BufferMVPData() {
    struct Matrices {
        Mat4 view;
        Mat4 projection;
    } matrices;
    for (int i = 0; i < uniformBuffers_.size(); i++) {
        auto& buffer = uniformBuffers_[i];
        memcpy(buffer->map, (void*)&projectionMat_, sizeof(Mat4));
        memcpy(((float*)buffer->map + 4 * 4), (void*)&viewMat_, sizeof(Mat4));
        TransBuffer2Device(*buffer, *deviceUniformBuffers_[i], buffer->size, 0, 0);
    }
}

void Renderer::SetDrawColor(const Color& color) {
    for (int i = 0; i < colorBuffers_.size(); i++) {
        auto& buffer = colorBuffers_[i];
        memcpy(buffer->map, (void*)&color, sizeof(float) * 3);
        TransBuffer2Device(*buffer, *deviceColorBuffers_[i], buffer->size, 0, 0);
    }
}

void Renderer::UpdateDescriptorSets() {
    for (int i = 0; i < descriptorSets_.size(); i++) {
        // bind MVP buffer
        vk::DescriptorBufferInfo bufferInfo1;
        bufferInfo1
            .setBuffer(deviceUniformBuffers_[i]->buffer)
            .setOffset(0)
            .setRange(sizeof(Mat4) * 2);

        std::vector<vk::WriteDescriptorSet> writerInfos(2);
        writerInfos[0]
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDstSet(descriptorSets_[i].set)
            .setDstBinding(0)
            .setDstArrayElement(0)
            .setDescriptorCount(1)
            .setBufferInfo(bufferInfo1);

        // bind color buffer
        vk::DescriptorBufferInfo bufferInfo2;
        bufferInfo2
            .setBuffer(deviceColorBuffers_[i]->buffer)
            .setOffset(0)
            .setRange(sizeof(Color));

        writerInfos[1]
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDstSet(descriptorSets_[i].set)
            .setDstBinding(1)
            .setDstArrayElement(0)
            .setDescriptorCount(1)
            .setBufferInfo(bufferInfo2);

        Context::GetInstance().device.updateDescriptorSets(writerInfos, {});
    }
}

void Renderer::SetProjectionMatrix(int left, int right, int top, int bottom, int near, int far) {
    projectionMat_ = Mat4::CreateOrtho(left, right, top, bottom, near, far);
    BufferMVPData();
}

std::uint32_t QueryBufferMemTypeIndex(std::uint32_t type, vk::MemoryPropertyFlags flag) {
    auto property = Context::GetInstance().phyDevice.getMemoryProperties();
    for (std::uint32_t i = 0; i < property.memoryTypeCount; i++) {
        if ((1 << i) & type &&
            property.memoryTypes[i].propertyFlags & flag) {
            return i;
        }
    }
    return 0;
}

} // namespace Vklib