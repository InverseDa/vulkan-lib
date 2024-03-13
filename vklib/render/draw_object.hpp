#ifndef VULKAN_LIB_DRAW_OBJECT_HPP
#define VULKAN_LIB_DRAW_OBJECT_HPP

#include "vulkan/vulkan.hpp"
#include <vector>

#include "math/vector.hpp"
#include "mesh/vertex.hpp"
#include "buffer/buffer.hpp"

namespace Vklib {
class DrawObject2D {
  public:
    DrawObject2D(std::vector<Vertex2>& vertices, std::vector<std::uint32_t>& indices);
    virtual void Draw();

  protected:
    std::vector<Vertex2> vertices;
    std::vector<std::uint32_t> indices;

    std::unique_ptr<Buffer> vertexBuffers_;
    std::unique_ptr<Buffer> indexBuffers_;

    std::vector<std::unique_ptr<Buffer>> uniformBuffers_;
    std::vector<std::unique_ptr<Buffer>> deviceUniformBuffers_;

    virtual void CreateVertexAndIndexBuffer();
    virtual void CreateUniformBuffer();
    virtual void BufferVertexAndIndexData();
    virtual void BufferUniformData();
};

class DrawObject3D {
  public:
    DrawObject3D(std::vector<Vertex3>& vertices, std::vector<std::uint32_t>& indices);
    virtual void Draw();

  protected:
    std::vector<Vertex3> vertices;
    std::vector<std::uint32_t> indices;

    std::unique_ptr<Buffer> vertexBuffers_;
    std::unique_ptr<Buffer> indexBuffers_;

    std::vector<std::unique_ptr<Buffer>> uniformBuffers_;
    std::vector<std::unique_ptr<Buffer>> deviceUniformBuffers_;

    virtual void CreateVertexAndIndexBuffer();
    virtual void CreateUniformBuffer();
    virtual void BufferVertexAndIndexData();
    virtual void BufferUniformData();
};
} // namespace Vklib

#endif // VULKAN_LIB_DRAW_OBJECT_HPP
