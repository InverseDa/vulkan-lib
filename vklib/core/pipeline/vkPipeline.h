#ifndef VULKAN_LIB_VKPIPELINE_H
#define VULKAN_LIB_VKPIPELINE_H

#include "macro.h"
#include "vulkan/vulkan.h"

namespace Vulkan {
class Pipeline {
  public:
    Pipeline() = default;
    Pipeline(VkGraphicsPipelineCreateInfo& createInfo);
    Pipeline(VkComputePipelineCreateInfo& createInfo);
    Pipeline(Pipeline&& other) noexcept;
    ~Pipeline();

    // Getter
    DefineHandleTypeOperator;
    DefineAddressFunction;

    ResultType Create(VkGraphicsPipelineCreateInfo& createInfo);
    ResultType Create(VkComputePipelineCreateInfo& createInfo);

  private:
    VkPipeline handle;
};
} // namespace Vulkan

#endif // VULKAN_LIB_VKPIPELINE_H
