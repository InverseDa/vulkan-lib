#include "vkDescriptor.h"

namespace Vulkan {
DescriptorSetLayout::DescriptorSetLayout(VkDescriptorSetLayoutCreateInfo& createInfo) {
    Create(createInfo);
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) {
    MoveHandle;
}

DescriptorSetLayout::~DescriptorSetLayout() {
    DestroyHandleBy(vkDestroyDescriptorSetLayout);
}

ResultType DescriptorSetLayout::Create(VkDescriptorSetLayoutCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    VkResult result = vkCreateDescriptorSetLayout(Vulkan::Context::GetInstance().GetDevice(), &createInfo, nullptr, &handle);
    if (result) {
        outStream << std::format("[DescriptorSetLayout][ERROR] Failed to create a descriptor set layout! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

}