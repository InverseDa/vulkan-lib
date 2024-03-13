#ifndef VULKAN_LIB_DRAW_OBJECT_HPP
#define VULKAN_LIB_DRAW_OBJECT_HPP

#include "vulkan/vulkan.hpp"
#include <vector>

#include "math/vector.hpp"
#include "mesh/vertex.hpp"

class DrawObject2D {
  public:
    std::vector<Vertex2> vertices;
    std::vector<unsigned int> indices;

  private:
};

class DrawObject3D {
  public:
    std::vector<Vertex3> vertices;
    std::vector<std::uint32_t> indices;

  private:
};

#endif // VULKAN_LIB_DRAW_OBJECT_HPP
