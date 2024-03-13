#include "draw_object.hpp"
#include "renderer.hpp"

namespace Vklib {
DrawObject2D::DrawObject2D(std::vector<Vertex2>& vertices, std::vector<std::uint32_t>& indices) {
    this->vertices = vertices;
    this->indices = indices;
}

void DrawObject2D::CreateVertexAndIndexBuffer() {
    vertexBuffers_.reset(new Buffer(vk::BufferUsageFlagBits::eVertexBuffer,
                                    sizeof(Vertex2) * vertices.size(),
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    indexBuffers_.reset(new Buffer(vk::BufferUsageFlagBits::eVertexBuffer,
                                   sizeof(std::uint32_t) * indices.size(),
                                   vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
}

void DrawObject2D::CreateUniformBuffer() {
    struct UniformBufferObject {
        mat4 model;
        mat4 view;
        mat4 proj;
    };
    int flightCount = Renderer::GetMaxFlightCount();

    uniformBuffers_.resize(flightCount);
    for (auto& buffer : uniformBuffers_) {
        buffer.reset(new Buffer(vk::BufferUsageFlagBits::eTransferSrc,
                                sizeof(UniformBufferObject),
                                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    }
    deviceUniformBuffers_.resize(flightCount);
    for (auto& buffer : deviceUniformBuffers_) {
        buffer.reset(new Buffer(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer,
                                sizeof(UniformBufferObject),
                                vk::MemoryPropertyFlagBits::eDeviceLocal));
    }
}

DrawObject3D::DrawObject3D(std::vector<Vertex3>& vertices, std::vector<std::uint32_t>& indices) {
    this->vertices = vertices;
    this->indices = indices;
}

void DrawObject3D::CreateVertexAndIndexBuffer() {
    vertexBuffers_.reset(new Buffer(vk::BufferUsageFlagBits::eVertexBuffer,
                                    sizeof(Vertex3) * vertices.size(),
                                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    indexBuffers_.reset(new Buffer(vk::BufferUsageFlagBits::eVertexBuffer,
                                   sizeof(std::uint32_t) * indices.size(),
                                   vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
}

void DrawObject3D::CreateUniformBuffer() {
    struct UniformBufferObject {
        mat4 model;
        mat4 view;
        mat4 proj;
    };
    int flightCount = Renderer::GetMaxFlightCount();

    uniformBuffers_.resize(flightCount);
    for (auto& buffer : uniformBuffers_) {
        buffer.reset(new Buffer(vk::BufferUsageFlagBits::eTransferSrc,
                                sizeof(UniformBufferObject),
                                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    }
    deviceUniformBuffers_.resize(flightCount);
    for (auto& buffer : deviceUniformBuffers_) {
        buffer.reset(new Buffer(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer,
                                sizeof(UniformBufferObject),
                                vk::MemoryPropertyFlagBits::eDeviceLocal));
    }
}

}; // namespace Vklib
