#ifndef VULKAN_LIB_UNIFORM_HPP
#define VULKAN_LIB_UNIFORM_HPP

#include "vulkan/vulkan.hpp"
#include "math/float2.hpp"

using Size = float2;

struct Color final {
    float r, g, b;
};

struct Rect {
    float2 position;
    Size size;
};

#endif // VULKAN_LIB_UNIFORM_HPP
