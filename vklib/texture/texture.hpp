#ifndef VULKAN_LIB_TEXTURE_HPP
#define VULKAN_LIB_TEXTURE_HPP

#include "vulkan/vulkan.hpp"
#include <string_view>

#include "vklib/buffer/buffer.hpp"

namespace Vklib {
class Texture final {
  public:
    Texture(std::string_view filename);
    ~Texture();

    vk::Image image;
    vk::DeviceMemory memory;
    vk::ImageView view;

  private:
    void CreateImage(uint32_t w, uint32_t h);
    void CreateImageView();
    void AllocateMemory();
    uint32_t QueryImageMemoryIndex();
    void TransitionImageLayoutFromUndefine2Dst();
    void TransitionImageLayoutFromDst2Optimal();
    void TransformData2Image(Buffer&, uint32_t w, uint32_t h);
};

}
#endif // VULKAN_LIB_TEXTURE_HPP
