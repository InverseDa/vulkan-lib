#ifndef VULKAN_LIB_RENDER_PROCESS_HPP
#define VULKAN_LIB_RENDER_PROCESS_HPP

#include "vulkan/vulkan.hpp"

namespace Vklib {
class RenderProcess final {
  public:
    vk::Pipeline graphicsPipeline = nullptr;
    vk::PipelineLayout layout = nullptr;
    vk::RenderPass renderPass = nullptr;

    RenderProcess();
    ~RenderProcess();

    void RecreateGraphicsPipeline(const std::vector<char>& vertexSource, const std::vector<char>& fragSource);
    void RecreateRenderPass();

  private:
    vk::PipelineLayout CreateLayout();
    vk::Pipeline CreateGraphicsPipeline(const std::vector<char>& vertexSource, const std::vector<char>& fragSource);
    vk::RenderPass CreateRenderPass();
};
} // namespace Vklib

#endif // VULKAN_LIB_RENDER_PROCESS_HPP
