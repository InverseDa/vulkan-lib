#ifndef VULKAN_LIB_UNIFORM_HPP
#define VULKAN_LIB_UNIFORM_HPP

#include "vulkan/vulkan.hpp"

namespace Vklib {
struct Color final {
    float r, g, b;
};

struct Uniform final {
    Color color;

    static vk::DescriptorSetLayoutBinding GetBinding() {
        vk::DescriptorSetLayoutBinding binding;
        binding
            .setBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setStageFlags(vk::ShaderStageFlagBits::eFragment)
            .setDescriptorCount(1);
        return binding;
    }
};

} // namespace Vklib

#endif // VULKAN_LIB_UNIFORM_HPP
