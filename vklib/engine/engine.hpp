#ifndef VULKAN_LIB_ENGINE_HPP
#define VULKAN_LIB_ENGINE_HPP

#include "vulkan/vulkan.hpp"

#include "vklib/core/context.hpp"
#include "vklib/shader/shader.hpp"
#include "vklib/render/renderer.hpp"

namespace Engine {
void Init(const std::vector<const char*>& extensions, CreateSurfaceFunc func, int w, int h);
void Quit();

inline Vklib::Renderer& GetRenderer() {
    return *Vklib::Context::GetInstance().renderer;
}

} // namespace Engine

#endif // VULKAN_LIB_ENGINE_HPP
