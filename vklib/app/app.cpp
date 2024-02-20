#include "app/app.h"

namespace Vulkan {
Application::Application(uint32_t width, uint32_t height, bool fullScreen, bool isResizable, bool limitFrameRate, std::string title)
    : window({width, height}, fullScreen, isResizable, limitFrameRate, title) {
    CreatePipeLineLayout();
    CreatePipeline();
}

Application::~Application() {
}

void Application::CreatePipeLineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayout.Create(pipelineLayoutCreateInfo);
}

void Application::CreatePipeline() {
    static Vulkan::ShaderModule shaderVert("vertex.spv");
    static Vulkan::ShaderModule shaderFrag("fragment.spv");
    static VkPipelineShaderStageCreateInfo shaderStages[2] = {
        shaderVert.GetStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT),
        shaderFrag.GetStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT),
    };
    std::function<void()> Create = [&]() {
        Vulkan::GraphicsCreateInfoPack pipelineCreateInfoPack;
        pipelineCreateInfoPack.createInfo.layout = pipelineLayout;
        pipelineCreateInfoPack.createInfo.renderPass = renderPassWithFrameBuffers.renderPass;
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
    std::function<void()> Destroy = [&]() {
//        pipeline.~Pipeline();
    };
    Vulkan::Context::GetInstance().AddCallbackCreateSwapChain(Create);
    Vulkan::Context::GetInstance().AddCallbackDestroySwapChain(Destroy);
    Create();
}

int Application::Run(const std::function<void()>& func) {
    try {
        commandPool.AllocateBuffers(commandBuffer);
        while (!window.IsClose()) {
            window.ShowTitleFPS();
            fence.WaitAndReset();
            Vulkan::Context::GetInstance().SwapImage(imageIsAvailableSem);
            auto index = Vulkan::Context::GetInstance().GetCurrentImageIndex();
            // ...
            // ========================================
            // This Area is Mouse and keyboard input
            // ========================================
            // ...
            commandBuffer.SetBuffers(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&] {
                renderPassWithFrameBuffers.renderPass.SetCommands(commandBuffer, renderPassWithFrameBuffers.frameBuffers[index], {{}, windowSize}, clearColor, [&] {
                    func();
                });
            });
            // Submit command buffer
            Vulkan::Context::GetInstance().SubmitCommandBufferGraphics(commandBuffer, imageIsAvailableSem, renderingIsFinishedSem, fence);
            // Present image
            Vulkan::Context::GetInstance().PresentImage(renderingIsFinishedSem);
            window.PollEvents();
        }
        commandPool.FreeBuffers(commandBuffer);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}

} // namespace Vulkan