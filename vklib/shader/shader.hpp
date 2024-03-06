#ifndef VULKAN_LIB_SHADER_HPP
#define VULKAN_LIB_SHADER_HPP

#include "vulkan/vulkan.hpp"

namespace Vklib {
class Shader final {
  public:
    static void Init(const std::string& vertexSource, const std::string& fragSource);
    static void Quit();

    static Shader& GetInstance() {
        return *instance_;
    }

    vk::ShaderModule vertexModule;
    vk::ShaderModule fragmentModule;

    ~Shader();

    std::vector<vk::PipelineShaderStageCreateInfo> GetStage();

  private:
    static std::unique_ptr<Shader> instance_;
    std::vector<vk::PipelineShaderStageCreateInfo> stage_;

    Shader(const std::string& vertexSource, const std::string& fragSource);
    void InitStage();
};
} // namespace Vklib

#endif // VULKAN_LIB_SHADER_HPP
