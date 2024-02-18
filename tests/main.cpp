#include "window/window.h"
#include "synchronize/fence.h"
#include "synchronize/semaphore.h"
#include "command/command.h"

int main() {
    WindowWrapper window({800, 600}, false, true);

    Vulkan::Fence fence(VK_FENCE_CREATE_SIGNALED_BIT);
    Vulkan::Semaphore imageIsAvailableSem;
    Vulkan::Semaphore renderingIsFinishedSem;

    Vulkan::CommandBuffer commandBuffer;
    Vulkan::CommandPool commandPool(Vulkan::GraphicsBase::GetInstance().GetQueueFamilyIndexGraphics(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    commandPool.AllocateBuffers(commandBuffer);

    while (!window.IsClose()) {
        window.ShowTitleFPS();

        fence.WaitAndReset();
        Vulkan::GraphicsBase::GetInstance().SwapImage(imageIsAvailableSem);
        // TODO: do
        commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        // TODO: Rendering command
        commandBuffer.End();

        // Submit command buffer
        Vulkan::GraphicsBase::GetInstance().SubmitCommandBufferGraphics(commandBuffer, imageIsAvailableSem, renderingIsFinishedSem, fence);
        // Present image
        Vulkan::GraphicsBase::GetInstance().PresentImage(renderingIsFinishedSem);

        window.PollEvents();
    }
}