#include "vkPipeline.h"

namespace Vulkan {
Pipeline::Pipeline(VkGraphicsPipelineCreateInfo& createInfo) {
    Create(createInfo);
}

Pipeline::Pipeline(VkComputePipelineCreateInfo& createInfo) {
    Create(createInfo);
}

Pipeline::Pipeline(Pipeline&& other) noexcept {
    MoveHandle;
}

Pipeline::~Pipeline() {
//    Vulkan::Context::GetInstance().WaitDeviceIdle();
    DestroyHandleBy(vkDestroyPipeline);
}

ResultType Pipeline::Create(VkGraphicsPipelineCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    VkResult result = vkCreateGraphicsPipelines(Context::GetInstance().GetDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &handle);
    if (result) {
        outStream << std::format("[Pipeline][ERROR] Failed to create a graphics pipeline! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

ResultType Pipeline::Create(VkComputePipelineCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    VkResult result = vkCreateComputePipelines(Context::GetInstance().GetDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &handle);
    if (result) {
        outStream << std::format("[Pipeline][ERROR] Failed to create a compute pipeline! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

} // namespace Vulkan