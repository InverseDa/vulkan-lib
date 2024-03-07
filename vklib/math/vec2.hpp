#ifndef VULKAN_LIB_VEC2_HPP
#define VULKAN_LIB_VEC2_HPP

#include "vulkan/vulkan.hpp"

struct Vec2 final {
  public:
    union {
        struct {
            float x, y;
        };
        struct {
            float w, h;
        };
    };

    static vk::VertexInputAttributeDescription GetAttributeDescription();
    static vk::VertexInputBindingDescription GetBindingDescription();
};

#endif // VULKAN_LIB_VEC2_HPP
