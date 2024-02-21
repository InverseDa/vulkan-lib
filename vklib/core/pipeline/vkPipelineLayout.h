#ifndef VULKAN_LIB_VKPIPELINELAYOUT_H
#define VULKAN_LIB_VKPIPELINELAYOUT_H

#include "macro.h"
#include "vulkan/vulkan.h"

namespace Vulkan {
class PipelineLayout {
  public:
    PipelineLayout() = default;
    PipelineLayout(VkPipelineLayoutCreateInfo& createInfo);
    PipelineLayout(PipelineLayout&& other) noexcept;
    ~PipelineLayout();

    // Getter
    DefineHandleTypeOperator;
    DefineAddressFunction;

    ResultType Create(VkPipelineLayoutCreateInfo& createInfo);

  private:
    VkPipelineLayout handle;
};
} // namespace Vulkan

#endif // VULKAN_LIB_VKPIPELINELAYOUT_H
