#include "shader.hpp"
#include "core/context.hpp"

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
    std::vector<vk::DescriptorSetLayoutBinding> bindings(1);
    bindings[0]
        .setBinding(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex);
    createInfo.setBindings(bindings);

    layouts_.push_back(Context::GetInstance().device.createDescriptorSetLayout(createInfo));

    bindings.resize(1);
    bindings[0]
        .setBinding(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setStageFlags(vk::ShaderStageFlagBits::eFragment);
    createInfo.setBindings(bindings);
    layouts_.push_back(Context::GetInstance().device.createDescriptorSetLayout(createInfo));
}

void Shader::SetPushConstantRange(uint32_t offset, uint32_t size, vk::ShaderStageFlags stage) {
    ranges_.push_back(vk::PushConstantRange{stage, offset, size});
}

} // namespace Vklib
