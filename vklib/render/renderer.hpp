#ifndef VULKAN_LIB_RENDERER_HPP
#define VULKAN_LIB_RENDERER_HPP

#include "vulkan/vulkan.hpp"

namespace Vklib {
class Renderer final {
  public:
    Renderer();
    ~Renderer();

    void Render();

  private:
    vk::CommandPool cmdPool_;
    vk::CommandBuffer cmdBuf_;

    vk::Semaphore imageAvailable_;
    vk::Semaphore imageDrawFinish_;
    vk::Fence cmdAvailableFence_;

    void InitCmdPool();
    void AllocateCmdBuf();
    void CreateSems();
    void CreateFence();
};
} // namespace Vklib

#endif // VULKAN_LIB_RENDERER_HPP
