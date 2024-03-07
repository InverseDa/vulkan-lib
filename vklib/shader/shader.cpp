#include "shader.hpp"
#include "vklib/core/context.hpp"

namespace Vklib {

Shader::Shader(const std::vector<char>& fragCodeSrc, const std::vector<char>& vertCodeSrc) {
    vk::ShaderModuleCreateInfo vertexModuleCreateInfo, fragModuleCreateInfo;
    vertexModuleCreateInfo.codeSize = vertCodeSrc.size();
    vertexModuleCreateInfo.pCode = (std::uint32_t*)vertCodeSrc.data();
    fragModuleCreateInfo.codeSize = fragCodeSrc.size();
    fragModuleCreateInfo.pCode = (std::uint32_t*)fragCodeSrc.data();

    auto& device = Context::GetInstance().device;
    vertexModule_ = device.createShaderModule(vertexModuleCreateInfo);
    fragModule_ = device.createShaderModule(fragModuleCreateInfo);

    InitDescriptorSetLayouts();
}

Shader::~Shader() {
    auto& device = Context::GetInstance().device;
    for (auto& layout : layouts_) {
        device.destroyDescriptorSetLayout(layout);
    }
    layouts_.clear();
    device.destroyShaderModule(vertexModule_);
    device.destroyShaderModule(fragModule_);
}

void Shader::InitDescriptorSetLayouts() {
    vk::DescriptorSetLayoutCreateInfo createInfo;
    vk::DescriptorSetLayoutBinding binding;
    binding.setBinding(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex);
    createInfo.setBindings(binding);
    layouts_.push_back(Context::GetInstance().device.createDescriptorSetLayout(createInfo));

    binding.setBinding(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setStageFlags(vk::ShaderStageFlagBits::eFragment);
    createInfo.setBindings(binding);
    layouts_.push_back(Context::GetInstance().device.createDescriptorSetLayout(createInfo));
}

} // namespace Vklib
