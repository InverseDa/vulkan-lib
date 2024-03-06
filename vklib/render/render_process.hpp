#ifndef VULKAN_LIB_RENDER_PROCESS_HPP
#define VULKAN_LIB_RENDER_PROCESS_HPP

#include "vulkan/vulkan.hpp"

namespace Vklib {
class RenderProcess final {
  public:
    vk::Pipeline pipeline;
    vk::PipelineLayout layout;
    vk::RenderPass renderPass;

    ~RenderProcess();

    void InitLayout();
    void InitRenderPass();
    void InitPipeline(int width, int height);
};
} // namespace Vklib

#endif // VULKAN_LIB_RENDER_PROCESS_HPP
