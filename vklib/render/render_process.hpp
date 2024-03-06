#ifndef VULKAN_LIB_RENDER_PROCESS_HPP
#define VULKAN_LIB_RENDER_PROCESS_HPP

#include "vulkan/vulkan.hpp"

namespace Vklib {
class RenderProcess final {
  public:
    vk::Pipeline pipeline;

    void InitPipeline(int width, int height);
    void DestroyPipeline();
};
} // namespace Vklib

#endif // VULKAN_LIB_RENDER_PROCESS_HPP
