#ifndef VULKAN_LIB_SHADER_MGR_HPP
#define VULKAN_LIB_SHADER_MGR_HPP

#include "vulkan/vulkan.hpp"
#include <unordered_map>

#include "shader.hpp"
#include "mesh/vertex.hpp"

namespace Vklib {
class ShaderMgr final {
  public:
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_;

    static ShaderMgr& GetInstance() {
        if (!instance_) {
            instance_.reset(new ShaderMgr());
        }
        return *instance_;
    }

    static void Quit() {
        instance_.reset();
    }

    void Load(const std::string& name, const std::vector<char>& vertexSource, const std::vector<char>& fragSource);
    std::shared_ptr<Shader> Get(const std::string& name) const { return shaders_.at(name); };
    void SetUniform(const std::string& name, const std::string& uniformName, void* data, size_t size);

    static std::vector<vk::VertexInputAttributeDescription> GetVertex3AttributeDescription();
    static std::vector<vk::VertexInputBindingDescription> GetVertex3BindingDescription();

    static std::vector<vk::VertexInputAttributeDescription> GetVertex2AttributeDescription();
    static std::vector<vk::VertexInputBindingDescription> GetVertex2BindingDescription();

    ~ShaderMgr();

  private:
    ShaderMgr();

    static std::unique_ptr<ShaderMgr> instance_;
};
} // namespace Vklib

#endif // VULKAN_LIB_SHADER_MGR_HPP
