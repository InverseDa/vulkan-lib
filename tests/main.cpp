#include "window/window.h"
#include "synchronize/fence.h"
#include "synchronize/semaphore.h"
#include "command/command.h"
#include "core/vkRenderPass.h"
#include "core/vkPipeline.h"
#include "core/vkPipelineLayout.h"
#include "core/vkShader.h"
#include "type/pipeline/graphicsCreateInfoPack.h"

Vulkan::Pipeline pipeline;
Vulkan::PipelineLayout pipelineLayoutTriangle;
const VkExtent2D& windowSize = Vulkan::GraphicsBase::GetInstance().GetSwapChainCreateInfo().imageExtent;

const Vulkan::RenderPassWithFrameBuffers& GetRenderPassAndFrameBuffers();
void CreateLayout();
void CreatePipeline();

int main() {
    WindowWrapper window({800, 600}, false, true);

    const auto& [renderPass, framebuffers] = GetRenderPassAndFrameBuffers();
    CreateLayout();
    CreatePipeline();

    Vulkan::Fence fence(VK_FENCE_CREATE_SIGNALED_BIT);
    Vulkan::Semaphore imageIsAvailableSem;
    Vulkan::Semaphore renderingIsFinishedSem;

    Vulkan::CommandBuffer commandBuffer;
    Vulkan::CommandPool commandPool(Vulkan::GraphicsBase::GetInstance().GetQueueFamilyIndexGraphics(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    commandPool.AllocateBuffers(commandBuffer);

    VkClearValue clearColor = {0.2f, 0.3f, 0.3f, 1.0f};

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
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        renderPass.CmdEnd(commandBuffer);
        commandBuffer.End();

        // Submit command buffer
        Vulkan::GraphicsBase::GetInstance().SubmitCommandBufferGraphics(commandBuffer, imageIsAvailableSem, renderingIsFinishedSem, fence);
        // Present image
        Vulkan::GraphicsBase::GetInstance().PresentImage(renderingIsFinishedSem);

        window.PollEvents();
    }
}

const Vulkan::RenderPassWithFrameBuffers& GetRenderPassAndFrameBuffers() {
    static const auto& rpwf_screen = Vulkan::CreateRenderPassWithFrameBuffersScreen();
    return rpwf_screen;
}

void CreateLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutTriangle.Create(pipelineLayoutCreateInfo);
}

void CreatePipeline() {
    static Vulkan::ShaderModule shaderVert("vertex.spv");
    static Vulkan::ShaderModule shaderFrag("fragment.spv");
    static VkPipelineShaderStageCreateInfo shaderStages[2] = {
        shaderVert.GetStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT),
        shaderFrag.GetStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT),
    };
    auto Create = [] {
        Vulkan::GraphicsCreateInfoPack pipelineCreateInfoPack;
        pipelineCreateInfoPack.createInfo.layout = pipelineLayoutTriangle;
        pipelineCreateInfoPack.createInfo.renderPass = GetRenderPassAndFrameBuffers().renderPass;
        pipelineCreateInfoPack.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        pipelineCreateInfoPack.viewports.emplace_back(0.f, 0.f, float(windowSize.width), float(windowSize.height), 0.f, 1.f);
        pipelineCreateInfoPack.scissors.emplace_back(VkOffset2D{}, windowSize);
        pipelineCreateInfoPack.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipelineCreateInfoPack.colorBlendAttachments.push_back({.colorWriteMask = 0b1111});
        pipelineCreateInfoPack.UpdateAllArrays();
        pipelineCreateInfoPack.createInfo.stageCount = 2;
        pipelineCreateInfoPack.createInfo.pStages = shaderStages;
        pipeline.Create(pipelineCreateInfoPack);
    };
    auto Destroy = [] {
        pipeline.~Pipeline();
    };
    Vulkan::GraphicsBase::GetInstance().AddCallbackCreateSwapChain(Create);
    Vulkan::GraphicsBase::GetInstance().AddCallbackDestroySwapChain(Destroy);
    Create();
}