#ifndef VULKAN_LIB_VERTEX_HPP
#define VULKAN_LIB_VERTEX_HPP

#include "vulkan/vulkan.hpp"

namespace Vklib {
struct Vertex final {
  public:
    float x, y;

    static vk::VertexInputAttributeDescription GetAttribute() {
        vk::VertexInputAttributeDescription attr;
        attr.setBinding(0)
            .setFormat(vk::Format::eR32G32Sfloat)
            .setLocation(0)
            .setOffset(0);
        return attr;
    }

    static vk::VertexInputBindingDescription GetBinding() {
        vk::VertexInputBindingDescription binding;

        binding.setBinding(0)
            .setInputRate(vk::VertexInputRate::eVertex)
            .setStride(sizeof(Vertex));

        return binding;
    }
};
} // namespace Vklib

#endif // VULKAN_LIB_VERTEX_HPP
