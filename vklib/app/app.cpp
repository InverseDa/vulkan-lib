#include "app.hpp"
#include "core/context.hpp"
#include "swapchain/swapchain.hpp"
#include "global_info.hpp"

#include "system/simple_render_system.hpp"

std::unique_ptr<Application> Application::instance_ = nullptr;

Application::Application(const std::string& title, int width, int height) {
    InitVulkan(window_->extensions, window_->getSurfaceCallback, width, height);

    // Init global descriptor pool
    globalPool = ida::IdaDescriptorPool::Builder()
                     .SetMaxSets(ida::IdaSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .AddPoolSize(vk::DescriptorType::eUniformBuffer, ida::IdaSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .Build();
}

void Application::InitVulkan(std::vector<const char*>& extensions, GetSurfaceCallback cb, int windowWidth, int windowHeight) {
    ida::Context::Init(extensions, cb);
    ida::Context::GetInstance().InitVulkan(windowWidth, windowHeight);

    renderer_ = std::make_unique<ida::IdaRenderer>();
}

void Application::DestroyVulkan() {
    ida::Context::GetInstance().device.waitIdle();
    renderer_.reset();
    //    ida::TextureMgr::GetInstance().Clear();
    //    ida::DescriptorSetMgr::Quit();
    ida::Context::Quit();
}

Application::~Application() {}

int Application::Run() {
    std::vector<std::unique_ptr<ida::IdaBuffer>> uboBuffers(ida::IdaSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<ida::IdaBuffer>(
            sizeof(ida::GlobalUbo),
            1,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible);
        uboBuffers[i]->Map();
    }
    auto globalSetLayout = ida::IdaDescriptorSetLayout::Builder()
                               .AddBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eAllGraphics)
                               .Build();
    std::vector<vk::DescriptorSet> globalDescriptorSets(ida::IdaSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->GetDescriptorInfo();
        ida::IdaDescriptorWriter(*globalSetLayout, *globalPool)
            .WriteBuffer(0, &bufferInfo)
            .Build(globalDescriptorSets[i]);
    }

    // TODO: add a simple render system
    //    ida::SimpleRenderSystem simpleRenderSystem{
    //
    //    };

    auto currentTime = std::chrono::high_resolution_clock::now();
    window_->Run([&]() {
        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        renderer_->StartRender();

        renderer_->EndRender();
    });

    DestroyVulkan();
    return 0;
}
