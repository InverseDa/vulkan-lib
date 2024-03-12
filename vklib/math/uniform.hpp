#ifndef VULKAN_LIB_UNIFORM_HPP
#define VULKAN_LIB_UNIFORM_HPP

#include "vulkan/vulkan.hpp"
#include "math/vec2.hpp"

using Size = Vec2;

struct Color final {
    float r, g, b;
};

struct Rect {
    Vec2 position;
    Size size;
};

#endif // VULKAN_LIB_UNIFORM_HPP
