#ifndef VULKAN_LIB_ENGINE_HPP
#define VULKAN_LIB_ENGINE_HPP

#include "vulkan/vulkan.hpp"

#include "core/context.hpp"
#include "render/renderer.hpp"
#include "math/vec2.hpp"
#include "math/uniform.hpp"
#include "texture/texture.hpp"

namespace Vklib {
void Init(std::vector<const char*>& extensions, Context::GetSurfaceCallback cb, int windowWidth, int windowHeight);
void Quit();

Renderer* GetRenderer();
Texture* LoadTexture(const std::string& filename);
void DestroyTexture(Texture*);
void ResizeSwapchainImage(int w, int h);

} // namespace Vklib

#endif // VULKAN_LIB_ENGINE_HPP
