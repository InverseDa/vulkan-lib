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

    void CreateFences();
    void CreateSemaphores();
    void CreateCmdBuffers();
    void CreateVertexBuffer();
    void BufferVertexData();
};
} // namespace Vklib

#endif // VULKAN_LIB_RENDERER_HPP
