#include "engine.hpp"

namespace Engine {
void Init(const std::vector<const char*>& extensions, CreateSurfaceFunc func, int w, int h) {
    Vklib::Context::Init(extensions, func);
    Vklib::Context::GetInstance().InitSwapchain(w, h);
    Vklib::Shader::Init(ReadWholeFile("./vert.spv"), ReadWholeFile("./frag.spv"));
}

void Quit() {
    Vklib::Context::GetInstance().DestroySwapchain();
    Vklib::Shader::Quit();
    Vklib::Context::Quit();
}

} // namespace Engine