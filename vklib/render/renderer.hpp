#ifndef VULKAN_LIB_RENDERER_HPP
#define VULKAN_LIB_RENDERER_HPP

#include "vulkan/vulkan.hpp"
#include "vklib/buffer/buffer.hpp"

namespace Vklib {
class Renderer final {
  public:
    Renderer(int maxFightCount = 2);
    ~Renderer();

    // WARNING: temporary draw triangle
    // TODO: implement this function
    void Render();

  private:
    int maxFightCount_;
    int cur_Frame_;
    std::vector<vk::Fence> fences_;
    std::vector<vk::Semaphore> imageAvaliableSems_;
    std::vector<vk::Semaphore> renderFinishSems_;
    std::vector<vk::CommandBuffer> cmdBufs_;

    std::unique_ptr<Buffer> hostVertexBuffer_;
    std::unique_ptr<Buffer> deviceVertexBuffer_;
    std::vector<std::unique_ptr<Buffer>> hostUniformBuffer_;
    std::vector<std::unique_ptr<Buffer>> deviceUniformBuffer_;

    vk::DescriptorPool descriptorPool_;
    std::vector<vk::DescriptorSet> sets_;

    void CreateFences();
    void CreateSemaphores();
    void CreateCmdBuffers();
    void CreateVertexBuffer();
    void BufferVertexData();
    void CreateUniformBuffer();
    void BufferUniformData();
    void CreateDescriptorPool();
    void AllocateSets();
    void UpdateSets();

    void CopyBuffer(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset);
};
} // namespace Vklib

#endif // VULKAN_LIB_RENDERER_HPP
