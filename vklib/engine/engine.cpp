#include "engine.hpp"

namespace Vklib {
std::unique_ptr<Renderer> renderer_;

void Init(std::vector<const char*>& extensions, Context::GetSurfaceCallback cb, int windowWidth, int windowHeight) {
    Context::Init(extensions, cb);
    auto& ctx = Context::GetInstance();
    ctx.InitSwapchain(windowWidth, windowHeight);
    ctx.InitRenderProcess();
    ctx.InitGraphicsPipeline();
    ctx.swapchain->InitFramebuffers();
    ctx.InitCommandPool();

    renderer_ = std::make_unique<Renderer>();
}

void Quit() {
    Context::GetInstance().device.waitIdle();
    renderer_.reset();
    Context::Quit();
}

Renderer* GetRenderer() {
    return renderer_.get();
}

} // namespace Vklib