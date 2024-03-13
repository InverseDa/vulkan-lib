#ifndef VULKAN_LIB_VERTEX_HPP
#define VULKAN_LIB_VERTEX_HPP

#include "vulkan/vulkan.hpp"

#include "math/float2.hpp"
#include "math/float3.hpp"

struct Vertex2 final {
    float2 pos;
    float2 texCoord;
};

struct Vertex3 final {
    float3 position;
    float3 normal;
    float2 texCoord;
};

#endif // VULKAN_LIB_VERTEX_HPP
