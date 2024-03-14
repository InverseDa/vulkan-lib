#ifndef VULKAN_LIB_TEXTURE_HPP
#define VULKAN_LIB_TEXTURE_HPP

#include "vulkan/vulkan.hpp"
#include <string_view>

#include "buffer/buffer.hpp"
#include "descriptor/descriptor_mgr.hpp"

namespace Vklib {
class Texture final {
  public:
    friend class TextureMgr;
    ~Texture();

    vk::Image image;
    vk::DeviceMemory memory;
    vk::ImageView view;
    DescriptorSetMgr::SetInfo info;

  private:
    Texture(std::string_view filename);
    Texture(void* pixels, uint32_t w, uint32_t h);

    void CreateImage(uint32_t w, uint32_t h);
    void CreateImageView();
    void AllocateMemory();
    uint32_t QueryImageMemoryIndex();
    void TransitionImageLayoutFromUndefine2Dst();
    void TransitionImageLayoutFromDst2Optimal();
    void TransformData2Image(Buffer&, uint32_t w, uint32_t h);
    void UpdateDescriptorSet();

    void Init(void* pixels, uint32_t w, uint32_t h);
};

class TextureMgr final {
  public:
    static TextureMgr& GetInstance() {
        if (!instance_) {
            instance_.reset(new TextureMgr);
        }
        return *instance_;
    }

    Texture* Load(const std::string& filename);
    Texture* Create(void* pixels, uint32_t w, uint32_t h);
    void Destroy(Texture*);
    void Clear();

  private:
    static std::unique_ptr<TextureMgr> instance_;

    std::vector<std::unique_ptr<Texture>> textures_;
};

} // namespace Vklib
#endif // VULKAN_LIB_TEXTURE_HPP
