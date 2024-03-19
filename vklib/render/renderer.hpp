#ifndef VULKAN_LIB_RENDERER_HPP
#define VULKAN_LIB_RENDERER_HPP

#include "vulkan/vulkan.hpp"
#include "buffer/buffer.hpp"
#include "core/window.hpp"

#include <limits>
#include "glm/glm.hpp"

namespace ida {
class IdaRenderer final {
  public:
    IdaRenderer(IdaWindow& window);
    ~IdaRenderer();
    IdaRenderer(const IdaRenderer&) = delete;
    IdaRenderer& operator=(const IdaRenderer&) = delete;

    void StartRender();
    void EndRender();

  private:
    int curFrame_;
    uint32_t curImageIndex_;
    std::vector<vk::Fence> fences_;
    std::vector<vk::Semaphore> imageAvaliableSems_;
    std::vector<vk::Semaphore> renderFinishSems_;
    std::vector<vk::CommandBuffer> cmdBufs_;

    vk::Sampler sampler;

    void CreateFences();
    void CreateSemaphores();
    void CreateCmdBuffers();
    void CreateBuffers();

    void TransBuffer2Device(IdaBuffer& src, IdaBuffer& dst, size_t size, size_t srcOffset, size_t dstOffset);
};

} // namespace ida

#endif // VULKAN_LIB_RENDERER_HPP
