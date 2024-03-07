#ifndef VULKAN_LIB_RENDER_PROCESS_HPP
#define VULKAN_LIB_RENDER_PROCESS_HPP

#include "vulkan/vulkan.hpp"
#include "vklib/shader/shader.hpp"

namespace Vklib {
class RenderProcess final {
  public:
    vk::Pipeline graphicsPipeline = nullptr;
    vk::RenderPass renderPass = nullptr;
    vk::PipelineLayout layout = nullptr;

    RenderProcess();
    ~RenderProcess();

    void RecreateGraphicsPipeline(const Shader& shader);
    void RecreateRenderPass();

  private:
    vk::PipelineLayout CreateLayout();
    vk::Pipeline CreateGraphicsPipeline(const Shader& shader);
    vk::RenderPass CreateRenderPass();
};
} // namespace Vklib

#endif // VULKAN_LIB_RENDER_PROCESS_HPP
