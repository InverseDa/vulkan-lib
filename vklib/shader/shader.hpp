#ifndef VULKAN_LIB_SHADER_HPP
#define VULKAN_LIB_SHADER_HPP

#include "vulkan/vulkan.hpp"

namespace Vklib {
class Shader final {
  public:
    Shader(const std::vector<char>& fragCodeSrc, const std::vector<char>& vertCodeSrc);
    ~Shader();

    [[nodiscard]] vk::ShaderModule GetVertexModule() const { return vertexModule_; }
    [[nodiscard]] vk::ShaderModule GetFragModule() const { return fragModule_; }
    [[nodiscard]] const std::vector<vk::DescriptorSetLayout>& GetDescriptorSetLayouts() const { return layouts_; }
    [[nodiscard]] std::vector<vk::PushConstantRange> GetPushConstantRange() const { return ranges_; };

    void SetPushConstantRange(uint32_t offset, uint32_t size, vk::ShaderStageFlags stage);

  private:
    vk::ShaderModule vertexModule_;
    vk::ShaderModule fragModule_;
    std::vector<vk::DescriptorSetLayout> layouts_;
    std::vector<vk::PushConstantRange> ranges_;

    void InitDescriptorSetLayouts();
};
} // namespace Vklib

#endif // VULKAN_LIB_SHADER_HPP
