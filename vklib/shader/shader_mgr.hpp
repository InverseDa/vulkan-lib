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

    static std::vector<vk::VertexInputAttributeDescription> GetVertex3AttributeDescription();
    static std::vector<vk::VertexInputBindingDescription> GetVertex3BindingDescription();

    static std::vector<vk::VertexInputAttributeDescription> GetVertex2AttributeDescription();
    static std::vector<vk::VertexInputBindingDescription> GetVertex2BindingDescription();

    void SetDescriptorSetLayoutBinding(const std::string& setName, const std::string& descriptorName, vk::DescriptorType type, vk::ShaderStageFlags stage, int binding, int count);
    void CreateDescriptorSetLayout(const std::string& setName);
    vk::DescriptorSetLayout GetDescriptorSetLayout(const std::string& setName) const { return descriptorSetLayouts.at(setName);};


    ~ShaderMgr();

  private:
    static std::unique_ptr<ShaderMgr> instance_;
    std::unordered_map<std::string, vk::DescriptorSetLayout> descriptorSetLayouts;
    // setname -> descriptorname -> layoutbinding
    std::unordered_map<std::string, std::unordered_map<std::string, vk::DescriptorSetLayoutBinding>> descriptorSetLayoutBindings;

    ShaderMgr();
};
} // namespace Vklib

#endif // VULKAN_LIB_SHADER_MGR_HPP
