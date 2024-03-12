#ifndef VULKAN_LIB_VERTEX_HPP
#define VULKAN_LIB_VERTEX_HPP

#include "vulkan/vulkan.hpp"

#include "math/vec2.hpp"

struct Vertex final {
    Vec2 pos;
    Vec2 texCoord;
};

#endif // VULKAN_LIB_VERTEX_HPP
