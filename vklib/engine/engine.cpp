#include "engine.hpp"

namespace Vklib {
std::unique_ptr<Renderer> renderer_;

void Init(std::vector<const char*>& extensions, Context::GetSurfaceCallback cb, int windowWidth, int windowHeight) {
    Context::Init(extensions, cb);
    auto& ctx = Context::GetInstance();
    ctx.InitSwapchain(windowWidth, windowHeight);
    ctx.InitShaderModules();
    ctx.InitRenderProcess();
    ctx.InitGraphicsPipeline();
    ctx.swapchain->InitFramebuffers();
    ctx.InitCommandPool();
    ctx.InitSampler();

    int maxFlightCount = 2;
    DescriptorSetMgr::Init(maxFlightCount);
    renderer_ = std::make_unique<Renderer>(maxFlightCount);
    renderer_->SetProjectionMatrix(0, windowWidth, windowHeight, 0, 1, -1);
}

void Quit() {
    Context::GetInstance().device.waitIdle();
    renderer_.reset();
    TextureMgr::GetInstance().Clear();
    DescriptorSetMgr::Quit();
    Context::Quit();
}

Texture* LoadTexture(const std::string& filename) {
    return TextureMgr::GetInstance().Load(filename);
}

void DestroyTexture(Texture* tex) {
    TextureMgr::GetInstance().Destroy(tex);
}

Renderer* GetRenderer() {
    return renderer_.get();
}

void ResizeSwapchainImage(int w, int h) {
    auto& ctx = Context::GetInstance();
    ctx.device.waitIdle();

    ctx.swapchain.reset();
    ctx.GetSurface();
    ctx.InitSwapchain(w, h);
    ctx.swapchain->InitFramebuffers();
}

} // namespace Vklib