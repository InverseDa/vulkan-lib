#ifndef VULKAN_LIB_APPLICATION_H
#define VULKAN_LIB_APPLICATION_H

#include "core/context/vkContext.h"
#include "core/framebuffer/vkFrameBuffer.h"
#include "core/renderpass/vkRenderPass.h"
#include "core/pipeline/vkPipeline.h"
#include "core/pipeline/vkPipelineLayout.h"
#include "core/shader/vkShader.h"
#include "window/window.h"
#include "synchronize/vkFence.h"
#include "synchronize/vkSemaphore.h"
#include "command/vkCommand.h"
#include "type/pipeline/graphicsCreateInfoPack.h"
#include "glm/glm.hpp"

#include <vector>
#include <functional>

namespace Vulkan {
class Application {
  public:
    Application(uint32_t width, uint32_t height, bool fullScreen = false, bool isResizable = true, bool limitFrameRate = true, std::string title = "Vulkan Application");
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;
    ~Application();

    void CreatePipeLineLayout();
    void CreatePipeline();

    int Run(const std::function<void()>& func);

    // GetterRe
    VkCommandBuffer GetCommandBuffer() const { return commandBuffer; }
    VkPipeline GetPipeline() const { return pipeline; }

  private:
    WindowWrapper window;

    Vulkan::Pipeline pipeline;
    Vulkan::PipelineLayout pipelineLayout;

    Vulkan::Fence fence = Vulkan::Fence(VK_FENCE_CREATE_SIGNALED_BIT);
    Vulkan::Semaphore imageIsAvailableSem;
    Vulkan::Semaphore renderingIsFinishedSem;

    Vulkan::CommandBuffer commandBuffer;
    Vulkan::CommandPool commandPool = Vulkan::CommandPool(Vulkan::Context::GetInstance().GetQueueFamilyIndexGraphics(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    const RenderPassWithFrameBuffers& renderPassWithFrameBuffers = Vulkan::CreateRenderPassWithFrameBuffersScreen();

    VkClearValue clearColor = {0.2f, 0.3f, 0.3f, 1.0f};
    const VkExtent2D& windowSize = Vulkan::Context::GetInstance().GetSwapChainCreateInfo().imageExtent;
};
} // namespace Vulkan

#endif
