#include "texture.hpp"
#include "core/context.hpp"

namespace Vklib {
std::unique_ptr<TextureMgr> TextureMgr::instance_ = nullptr;

Texture* TextureMgr::Load(const std::string& filename) {
    textures_.push_back(std::unique_ptr<Texture>(new Texture(filename)));
    return textures_.back().get();
}

Texture* TextureMgr::Create(void* pixels, uint32_t w, uint32_t h) {
    textures_.push_back(std::unique_ptr<Texture>(new Texture(pixels, w, h)));
    return textures_.back().get();
}

void TextureMgr::Clear() {
    textures_.clear();
}

void TextureMgr::Destroy(Texture* texture) {
    auto it = std::find_if(textures_.begin(), textures_.end(), [&](const std::unique_ptr<Texture>& t) {
        return t.get() == texture;
    });
    if (it != textures_.end()) {
        Context::GetInstance().device.waitIdle();
        textures_.erase(it);
        return;
    }
}

} // namespace Vklib