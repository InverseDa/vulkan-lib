#include "command.hpp"
#include "core/context.hpp"

namespace ida {
CommandMgr::CommandMgr() {
    pool_ = CreateCommandPool();
}

CommandMgr::~CommandMgr() {
    Context::GetInstance().device.destroyCommandPool(pool_);
}

vk::CommandPool CommandMgr::CreateCommandPool() {
    auto& ctx = Context::GetInstance();
    auto createInfo = vk::CommandPoolCreateInfo()
                          .setQueueFamilyIndex(ctx.queueInfo.graphicsIndex.value())
                          .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    return ctx.device.createCommandPool(createInfo);
}

void CommandMgr::ExecuteCommand(vk::Queue queue, std::function<void(vk::CommandBuffer&)> func) {
    auto cmd = CreateCommandBuffers(1)[0];
    auto& ctx = Context::GetInstance();
    auto beginInfo = vk::CommandBufferBeginInfo()
                         .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    cmd.begin(beginInfo);
    if (func)
        func(cmd);
    cmd.end();

    auto submitInfo = vk::SubmitInfo()
                          .setCommandBuffers(cmd);
    queue.submit(submitInfo);
    queue.waitIdle();
    ctx.device.waitIdle();
    FreeCommand(cmd);
}

std::vector<vk::CommandBuffer> CommandMgr::CreateCommandBuffers(std::uint32_t count) {
    auto& ctx = Context::GetInstance();
    auto allocateInfo = vk::CommandBufferAllocateInfo()
                            .setCommandPool(pool_)
                            .setCommandBufferCount(count)
                            .setLevel(vk::CommandBufferLevel::ePrimary);
    return ctx.device.allocateCommandBuffers(allocateInfo);
}

void CommandMgr::FreeCommand(const vk::CommandBuffer& cmd) {
    Context::GetInstance().device.freeCommandBuffers(pool_, cmd);
}

}; // namespace ida