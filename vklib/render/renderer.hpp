#ifndef VULKAN_LIB_RENDERER_HPP
#define VULKAN_LIB_RENDERER_HPP

#include "vulkan/vulkan.hpp"
#include "vklib/buffer/buffer.hpp"
#include "vklib/math/uniform.hpp"
#include "vklib/math/mat4.hpp"
#include <limits>

namespace Vklib {
class Renderer final {
  public:
    Renderer(int maxFightCount = 2);
    ~Renderer();

    // TODO: temporary draw rect, implement this function
    void Render(const Rect& rect);
    void SetDrawColor(const Color& color);
    void SetProjectionMatrix(int left, int right, int top, int bottom, int near, int far);

  private:
    int maxFightCount_;
    int cur_Frame_;
    std::vector<vk::Fence> fences_;
    std::vector<vk::Semaphore> imageAvaliableSems_;
    std::vector<vk::Semaphore> renderFinishSems_;
    std::vector<vk::CommandBuffer> cmdBufs_;
    std::unique_ptr<Buffer> verticesBuffer_;
    std::unique_ptr<Buffer> indicesBuffer_;

    Mat4 projectionMat_;
    Mat4 viewMat_;

    std::vector<std::unique_ptr<Buffer>> uniformBuffers_;
    std::vector<std::unique_ptr<Buffer>> colorBuffers_;
    std::vector<std::unique_ptr<Buffer>> deviceUniformBuffers_;
    std::vector<std::unique_ptr<Buffer>> deviceColorBuffers_;

    vk::DescriptorPool descriptorPool_;

    std::vector<vk::DescriptorSet> descriptorSets_;

    void InitMats();

    void CreateFences();
    void CreateSemaphores();
    void CreateCmdBuffers();
    void CreateBuffers();
    void CreateUniformBuffers(int flightCount);
    void BufferVertexData();
    void BufferIndicesData();
    void BufferMVPData();
    void BufferData();
    void CreateDescriptorPool(int flightCount);
    std::vector<vk::DescriptorSet> AllocateDescriptorSet(int flightCount);
    void AllocateDescriptorSets(int flightCount);
    void UpdateDescriptorSets();
    void TransBuffer2Device(Buffer& src, Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset);

    std::uint32_t QueryBufferMemTypeIndex(std::uint32_t, vk::MemoryPropertyFlags);
};
} // namespace Vklib

#endif // VULKAN_LIB_RENDERER_HPP
