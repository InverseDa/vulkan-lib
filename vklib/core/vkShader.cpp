#include "core/vkShader.h"

namespace Vulkan {

ShaderModule::ShaderModule(VkShaderModuleCreateInfo& createInfo) {
    Create(createInfo);
}

ShaderModule::ShaderModule(const char* filePath) {
    Create(filePath);
}

ShaderModule::ShaderModule(size_t codeSize, const uint32_t* pCode) {
    Create(codeSize, pCode);
}

ShaderModule::ShaderModule(ShaderModule&& other) noexcept {
    MoveHandle;
}

ShaderModule::~ShaderModule() {
    DestroyHandleBy(vkDestroyShaderModule);
}

VkPipelineShaderStageCreateInfo ShaderModule::GetStageCreateInfo(VkShaderStageFlagBits stage, const char* entry) const {
    return {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = stage,
        .module = handle,
        .pName = entry,
        .pSpecializationInfo = nullptr,
    };
}

ResultType ShaderModule::Create(VkShaderModuleCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    VkResult result = vkCreateShaderModule(Context::GetInstance().GetDevice(), &createInfo, nullptr, &handle);
    if (result) {
        outStream << std::format("[ShaderModule][ERROR] Failed to create a shader module! Error: {}({})\n", string_VkResult(result), int32_t(result));
    }
    return result;
}

ResultType ShaderModule::Create(const char* filePath) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file) {
        outStream << std::format("[ShaderModule][ERROR] Failed to open file: {}\n", filePath);
        return VK_RESULT_MAX_ENUM;
    }
    size_t fileSize = size_t(file.tellg());
    std::vector<uint32_t> binaries(fileSize / 4);
    file.seekg(0);
    file.read(reinterpret_cast<char*>(binaries.data()), fileSize);
    file.close();
    return Create(fileSize, binaries.data());
}

ResultType ShaderModule::Create(size_t codeSize, const uint32_t* pCode) {
    VkShaderModuleCreateInfo createInfo = {
        .codeSize = codeSize,
        .pCode = pCode,
    };
    return Create(createInfo);
}

} // namespace Vulkan