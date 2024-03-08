#include "renderer.hpp"
#include "vklib/core/context.hpp"
#include "vklib/math/vec2.hpp"
#include "vklib/math/uniform.hpp"

namespace Vklib {

Renderer::Renderer(int maxFlightCount) : maxFightCount_(maxFlightCount), cur_Frame_(0) {
    CreateFences();
    CreateSemaphores();
    CreateCmdBuffers();
    CreateBuffers();
    CreateUniformBuffers(maxFlightCount);
    BufferData();
    CreateDescriptorPool(maxFlightCount);
    AllocateDescriptorSets(maxFlightCount);
    UpdateDescriptorSets();
    InitMats();
}

Renderer::~Renderer() {
    auto& device = Context::GetInstance().device;
    device.destroyDescriptorPool(descriptorPool1_);
    device.destroyDescriptorPool(descriptorPool2_);
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

void Renderer::Render(const Rect& rect) {
    auto& ctx = Context::GetInstance();
    auto& device = ctx.device;
    if (device.waitForFences(fences_[cur_Frame_], true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess) {
        IO::ThrowError("Wait for fence failed!");
    }
    device.resetFences(fences_[cur_Frame_]);

    auto model = Mat4::CreateTranslate(rect.position) * Mat4::CreateScale(rect.size);
    BufferMVPData(model);

    auto& swapchain = ctx.swapchain;
    auto resultValue = device.acquireNextImageKHR(swapchain->swapchain,
                                                  std::numeric_limits<std::uint64_t>::max(),
                                                  imageAvaliableSems_[cur_Frame_],
                                                  nullptr);
    if (resultValue.result != vk::Result::eSuccess) {
        IO::ThrowError("Wait for image in swapchain failed!");
    }
    auto imageIndex = resultValue.value;

    auto& cmdMgr = ctx.commandMgr;
    auto& cmd = cmdBufs_[cur_Frame_];
    cmd.reset();

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmd.begin(beginInfo);
    {
        vk::ClearValue clearValue;
        clearValue.setColor(vk::ClearColorValue(std::array<float, 4>{0.2, 0.3, 0.3, 1}));
        vk::RenderPassBeginInfo renderPassBeginInfo;
        renderPassBeginInfo
            .setRenderPass(ctx.renderProcess->renderPass)
            .setFramebuffer(swapchain->framebuffers[imageIndex])
            .setRenderArea(vk::Rect2D({}, swapchain->GetExtent()))
            .setClearValues(clearValue);

        cmd.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
        {
            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, ctx.renderProcess->graphicsPipeline);
            vk::DeviceSize offset = 0;
            cmd.bindVertexBuffers(0, verticesBuffer_->buffer, offset);
            cmd.bindIndexBuffer(indicesBuffer_->buffer, 0, vk::IndexType::eUint32);
            cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                   Context::GetInstance().renderProcess->layout,
                                   0,
                                   descriptorSets_.first[cur_Frame_],
                                   {});
            cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                   Context::GetInstance().renderProcess->layout,
                                   1,
                                   descriptorSets_.second[cur_Frame_],
                                   {});

            cmd.drawIndexed(6, 1, 0, 0, 0);
        }
        cmd.endRenderPass();
    }
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

void Renderer::CreateBuffers() {
    auto& device = Context::GetInstance().device;
    verticesBuffer_.reset(new Buffer(vk::BufferUsageFlagBits::eVertexBuffer,
                                     sizeof(float) * 8,
                                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    indicesBuffer_.reset(new Buffer(vk::BufferUsageFlagBits::eIndexBuffer,
                                    sizeof(float) * 6,
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
}

void Renderer::CreateUniformBuffers(int flightCount) {
    uniformBuffers_.resize(flightCount);
    // including MVP and color, 4 * 4 * 3
    size_t size = sizeof(float) * 4 * 4 * 3;
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
    size = sizeof(float) * 3;
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
    auto cmdBuf = Context::GetInstance().commandMgr->CreateACommandBuffer();

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuf.begin(beginInfo);
    {
        vk::BufferCopy region;
        region
            .setSize(size)
            .setSrcOffset(srcOffset)
            .setDstOffset(dstOffset);
        cmdBuf.copyBuffer(src.buffer, dst.buffer, region);
    }
    cmdBuf.end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(cmdBuf);
    Context::GetInstance().graphicsQueue.submit(submitInfo);
    Context::GetInstance().graphicsQueue.waitIdle();
    Context::GetInstance().device.waitIdle();
    Context::GetInstance().commandMgr->FreeCmd(cmdBuf);
}

std::uint32_t Renderer::QueryBufferMemTypeIndex(std::uint32_t type, vk::MemoryPropertyFlags flag) {
    auto property = Context::GetInstance().phyDevice.getMemoryProperties();
    for (std::uint32_t i = 0; i < property.memoryTypeCount; i++) {
        if ((1 << i) & type &&
            property.memoryTypes[i].propertyFlags & flag) {
            return i;
        }
    }
    return 0;
}

void Renderer::BufferData() {
    BufferVertexData();
    BufferIndicesData();
}

void Renderer::BufferVertexData() {
    Vec2 vertices[] = {
        {-0.5, -0.5},
        {0.5, -0.5},
        {0.5, 0.5},
        {-0.5, 0.5},
    };
    memcpy(verticesBuffer_->map, vertices, sizeof(vertices));
}

void Renderer::BufferIndicesData() {
    std::uint32_t indices[] = {
        0,
        1,
        2,
        2,
        3,
        0,
    };
    memcpy(indicesBuffer_->map, indices, sizeof(indices));
}

void Renderer::BufferMVPData(const Mat4& model) {
    MVP mvp;
    mvp.projection = projectionMat_;
    mvp.view = viewMat_;
    mvp.model = model;
    for (int i = 0; i < uniformBuffers_.size(); i++) {
        auto& buffer = uniformBuffers_[i];
        memcpy(buffer->map, (void*)&mvp, sizeof(mvp));
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

void Renderer::CreateDescriptorPool(int flightCount) {
    vk::DescriptorPoolCreateInfo createInfo;
    vk::DescriptorPoolSize poolSize;
    poolSize.setType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(flightCount);

    std::vector<vk::DescriptorPoolSize> sizes(2, poolSize);
    createInfo.setMaxSets(flightCount)
        .setPoolSizes(sizes);
    descriptorPool1_ = Context::GetInstance().device.createDescriptorPool(createInfo);
    descriptorPool2_ = Context::GetInstance().device.createDescriptorPool(createInfo);
}

void Renderer::AllocateDescriptorSets(int flightCount) {
    std::vector<vk::DescriptorSetLayout> layouts(flightCount, Context::GetInstance().shader->GetDescriptorSetLayouts()[0]);
    vk::DescriptorSetAllocateInfo allocateInfo;
    allocateInfo
        .setDescriptorPool(descriptorPool1_)
        .setSetLayouts(layouts);
    descriptorSets_.first = Context::GetInstance().device.allocateDescriptorSets(allocateInfo);

    layouts = std::vector(flightCount, Context::GetInstance().shader->GetDescriptorSetLayouts()[1]);
    allocateInfo
        .setDescriptorPool(descriptorPool2_)
        .setSetLayouts(layouts);
    descriptorSets_.second = Context::GetInstance().device.allocateDescriptorSets(allocateInfo);
}

void Renderer::UpdateDescriptorSets() {
    for (int i = 0; i < descriptorSets_.first.size(); i++) {
        // bind MVP buffer
        vk::DescriptorBufferInfo bufferInfo1;
        bufferInfo1
            .setBuffer(deviceUniformBuffers_[i]->buffer)
            .setOffset(0)
            .setRange(sizeof(float) * 4 * 4 * 3);

        std::vector<vk::WriteDescriptorSet> writerInfos(2);
        writerInfos[0]
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDstSet(descriptorSets_.first[i])
            .setDstBinding(0)
            .setDstArrayElement(0)
            .setDescriptorCount(1)
            .setBufferInfo(bufferInfo1);

        // bind color buffer
        vk::DescriptorBufferInfo bufferInfo2;
        bufferInfo2
            .setBuffer(deviceColorBuffers_[i]->buffer)
            .setOffset(0)
            .setRange(sizeof(float) * 3);

        writerInfos[1]
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDstSet(descriptorSets_.second[i])
            .setDstBinding(0)
            .setDstArrayElement(0)
            .setDescriptorCount(1)
            .setBufferInfo(bufferInfo2);

        Context::GetInstance().device.updateDescriptorSets(writerInfos, {});
    }
}

void Renderer::SetProjectionMatrix(int left, int right, int top, int bottom, int near, int far) {
    projectionMat_ = Mat4::CreateOrtho(left, right, top, bottom, near, far);
}

} // namespace Vklib