#include "engine.hpp"
#include "vklib/core/context.hpp"

namespace Engine {
void Init(const std::vector<const char*>& extensions, CreateSurfaceFunc func, int w, int h) {
    Vklib::Context::Init(extensions, func);
    Vklib::Context::GetInstance().InitSwapchain(w, h);
}

void Quit() {
    Vklib::Context::GetInstance().DestroySwapchain();
    Vklib::Context::Quit();
}

} // namespace Engine