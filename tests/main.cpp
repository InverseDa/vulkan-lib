#include "window/window.h"
#include "synchronize/fence.h"
#include "synchronize/semaphore.h"
#include "command/command.h"
#include "vkInstance/vkRenderPass.h"

int main() {
    WindowWrapper window({800, 600}, false, true);

    const auto& [renderPass, framebuffers] = Vulkan::CreateRenderPassWithFrameBuffersScreen();

    Vulkan::Fence fence(VK_FENCE_CREATE_SIGNALED_BIT);
    Vulkan::Semaphore imageIsAvailableSem;
    Vulkan::Semaphore renderingIsFinishedSem;

    Vulkan::CommandBuffer commandBuffer;
    Vulkan::CommandPool commandPool(Vulkan::GraphicsBase::GetInstance().GetQueueFamilyIndexGraphics(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    commandPool.AllocateBuffers(commandBuffer);

    VkClearValue clearColor = {0.2f, 0.3f, 0.3f, 1.0f};
    const VkExtent2D& windowSize = Vulkan::GraphicsBase::GetInstance().GetSwapChainCreateInfo().imageExtent;

    while (!window.IsClose()) {
        window.ShowTitleFPS();

        fence.WaitAndReset();
        Vulkan::GraphicsBase::GetInstance().SwapImage(imageIsAvailableSem);
        auto index = Vulkan::GraphicsBase::GetInstance().GetCurrentImageIndex();
        // ========================================
        // This Area is Mouse and keyboard input
        // ========================================
        // Begin command buffer
        commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        renderPass.CmdBegin(commandBuffer, framebuffers[index], {{}, windowSize}, clearColor);
        renderPass.CmdEnd(commandBuffer);
        commandBuffer.End();

        // Submit command buffer
        Vulkan::GraphicsBase::GetInstance().SubmitCommandBufferGraphics(commandBuffer, imageIsAvailableSem, renderingIsFinishedSem, fence);
        // Present image
        Vulkan::GraphicsBase::GetInstance().PresentImage(renderingIsFinishedSem);

        window.PollEvents();
    }
}