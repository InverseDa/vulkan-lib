#ifndef VULKAN_LIB_ENGINE_HPP
#define VULKAN_LIB_ENGINE_HPP

#include "vulkan/vulkan.hpp"

#include "vklib/core/context.hpp"

namespace Engine {
void Init(const std::vector<const char*>& extensions, CreateSurfaceFunc func, int w, int h);
void Quit();

} // namespace Engine

#endif // VULKAN_LIB_ENGINE_HPP
