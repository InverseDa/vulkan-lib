#include "engine.hpp"

namespace Engine {
void Init(const std::vector<const char*>& extensions, CreateSurfaceFunc func, int w, int h) {
    Vklib::Context::Init(extensions, func);
    Vklib::Context::GetInstance().InitSwapchain(w, h);
    Vklib::Shader::Init(ReadWholeFile("shaders/vert.spv"), ReadWholeFile("shaders/frag.spv"));
    Vklib::Context::GetInstance().renderProcess->InitRenderPass();
    Vklib::Context::GetInstance().renderProcess->InitLayout();
    Vklib::Context::GetInstance().swapchain->CreateFramebuffers(w, h);
    Vklib::Context::GetInstance().renderProcess->InitPipeline(w, h);
    Vklib::Context::GetInstance().InitRenderer();
}

void Quit() {
    Vklib::Context::GetInstance().device.waitIdle();
    Vklib::Context::GetInstance().renderer.reset();
    Vklib::Context::GetInstance().renderProcess.reset();
    Vklib::Context::GetInstance().DestroySwapchain();
    Vklib::Shader::Quit();
    Vklib::Context::Quit();
}

} // namespace Engine