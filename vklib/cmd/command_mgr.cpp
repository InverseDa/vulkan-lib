#include "command_mgr.hpp"
#include "vklib/core/context.hpp"

namespace Vklib {

CommandMgr::CommandMgr() {
    pool_ = CreateCommandPool();
}

CommandMgr::~CommandMgr() {
    auto& ctx = Context::GetInstance();
    ctx.device.destroyCommandPool(pool_);
}

void CommandMgr::ResetCmds() {
    Context::GetInstance().device.resetCommandPool(pool_);
}

vk::CommandPool CommandMgr::CreateCommandPool() {
    auto& ctx = Context::GetInstance();
    vk::CommandPoolCreateInfo commandPoolCreateInfo;

    commandPoolCreateInfo
        .setQueueFamilyIndex(ctx.queueInfo.graphicsIndex.value())
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    return ctx.device.createCommandPool(commandPoolCreateInfo);
}

std::vector<vk::CommandBuffer> CommandMgr::CreateCommandBuffers(std::uint32_t count) {
    auto& ctx = Context::GetInstance();
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo
        .setCommandPool(pool_)
        .setCommandBufferCount(count)
        .setLevel(vk::CommandBufferLevel::ePrimary);
    return ctx.device.allocateCommandBuffers(commandBufferAllocateInfo);
}

vk::CommandBuffer CommandMgr::CreateACommandBuffer() {
    return CreateCommandBuffers(1)[0];
}

void CommandMgr::FreeCmd(const vk::CommandBuffer& buf) {
    Context::GetInstance().device.freeCommandBuffers(pool_, buf);
}

} // namespace Vklib
