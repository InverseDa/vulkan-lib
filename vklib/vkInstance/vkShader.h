#ifndef VULKAN_LIB_VKSHADER_H
#define VULKAN_LIB_VKSHADER_H

#include "macro.h"
#include "vulkan/vulkan.h"

#include <fstream>

namespace Vulkan {
class ShaderModule {
  public:
    ShaderModule() = default;
    ShaderModule(VkShaderModuleCreateInfo& createInfo);
    ShaderModule(const char* filePath);
    ShaderModule(size_t codeSize, const uint32_t* pCode);
    ShaderModule(ShaderModule&& other) noexcept;
    ~ShaderModule();

    // Getter
    DefineHandleTypeOperator;
    DefineAddressFunction;

    VkPipelineShaderStageCreateInfo GetStageCreateInfo(VkShaderStageFlagBits stage, const char* entry = "main") const;

    ResultType Create(VkShaderModuleCreateInfo& createInfo);
    ResultType Create(const char* filePath);
    ResultType Create(size_t codeSize, const uint32_t* pCode);

  private:
    VkShaderModule handle = VK_NULL_HANDLE;
};
} // namespace Vulkan

#endif // VULKAN_LIB_VKSHADER_H
