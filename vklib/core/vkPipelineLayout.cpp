#include "core/vkPipelineLayout.h"

namespace Vulkan {
PipelineLayout::PipelineLayout(VkPipelineLayoutCreateInfo& createInfo) {
    Create(createInfo);
}

PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept {
    MoveHandle;
}

PipelineLayout::~PipelineLayout() {
    DestroyHandleBy(vkDestroyPipelineLayout);
}

ResultType PipelineLayout::Create(VkPipelineLayoutCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    VkResult result = vkCreatePipelineLayout(GraphicsBase::GetInstance().GetDevice(), &createInfo, nullptr, &handle);
    if (result) {
        outStream << std::format("Failed to create pipeline layout! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

} // namespace Vulkan
