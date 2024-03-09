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
    std::vector<vk::DescriptorSetLayoutBinding> bindings(3);
    bindings[0]
        .setBinding(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex);
    bindings[1]
        .setBinding(1)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setStageFlags(vk::ShaderStageFlagBits::eFragment);
    bindings[2]
        .setBinding(2)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setStageFlags(vk::ShaderStageFlagBits::eFragment);

    createInfo.setBindings(bindings);

    layouts_.push_back(Context::GetInstance().device.createDescriptorSetLayout(createInfo));
}

vk::PushConstantRange Shader::GetPushConstantRange() const {
    vk::PushConstantRange range;
    range
        .setOffset(0)
        .setSize(sizeof(Mat4))
        .setStageFlags(vk::ShaderStageFlagBits::eVertex);
    return range;
}

} // namespace Vklib
