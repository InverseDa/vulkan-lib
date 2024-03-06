#ifndef VULKAN_LIB_ENGINE_HPP
#define VULKAN_LIB_ENGINE_HPP

#include "vulkan/vulkan.hpp"

#include "vklib/core/context.hpp"
#include "vklib/render/renderer.hpp"

namespace Vklib {
void Init(std::vector<const char*>& extensions, Context::GetSurfaceCallback cb, int windowWidth, int windowHeight);
void Quit();

Renderer* GetRenderer();

} // namespace Vklib

#endif // VULKAN_LIB_ENGINE_HPP
