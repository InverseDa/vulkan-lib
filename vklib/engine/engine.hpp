#ifndef VULKAN_LIB_ENGINE_HPP
#define VULKAN_LIB_ENGINE_HPP

#include "vulkan/vulkan.hpp"

#include "vklib/core/context.hpp"
#include "vklib/render/renderer.hpp"
#include "vklib/math/vec2.hpp"
#include "vklib/math/uniform.hpp"
#include "vklib/texture/texture.hpp"

namespace Vklib {
void Init(std::vector<const char*>& extensions, Context::GetSurfaceCallback cb, int windowWidth, int windowHeight);
void Quit();

Renderer* GetRenderer();
Texture* LoadTexture(const std::string& filename);
void DestroyTexture(Texture*);

} // namespace Vklib

#endif // VULKAN_LIB_ENGINE_HPP
