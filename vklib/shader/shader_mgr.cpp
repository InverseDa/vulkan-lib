#include "shader_mgr.hpp"

namespace Vklib {
std::unique_ptr<ShaderMgr> ShaderMgr::instance_ = nullptr;

std::vector<vk::VertexInputAttributeDescription> ShaderMgr::GetVertex3AttributeDescription() {
    std::vector<vk::VertexInputAttributeDescription> descriptions(3);
    // position
    descriptions[0]
        .setBinding(0)
        .setFormat(vk::Format::eR32G32B32Sfloat)
        .setLocation(0)
        .setOffset(0);
    // normal
    descriptions[1]
        .setBinding(0)
        .setFormat(vk::Format::eR32G32B32Sfloat)
        .setLocation(1)
        .setOffset(offsetof(Vertex3, normal));
    // texcoord
    descriptions[2]
        .setBinding(0)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setLocation(2)
        .setOffset(offsetof(Vertex3, texCoord));
    return descriptions;
}

std::vector<vk::VertexInputBindingDescription> ShaderMgr::GetVertex3BindingDescription() {
    std::vector<vk::VertexInputBindingDescription> descriptions(1);
    descriptions[0]
        .setBinding(0)
        .setStride(sizeof(Vertex3))
        .setInputRate(vk::VertexInputRate::eVertex);
    return descriptions;
}

std::vector<vk::VertexInputAttributeDescription> ShaderMgr::GetVertex2AttributeDescription() {
    std::vector<vk::VertexInputAttributeDescription> descriptions(2);
    // position
    descriptions[0]
        .setBinding(0)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setLocation(0)
        .setOffset(0);
    // texcoord
    descriptions[1]
        .setBinding(0)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setLocation(1)
        .setOffset(offsetof(Vertex2, texCoord));
    return descriptions;
}

std::vector<vk::VertexInputBindingDescription> ShaderMgr::GetVertex2BindingDescription() {
    std::vector<vk::VertexInputBindingDescription> descriptions(1);
    descriptions[0]
        .setBinding(0)
        .setStride(sizeof(Vertex2))
        .setInputRate(vk::VertexInputRate::eVertex);
    return descriptions;
}

ShaderMgr::ShaderMgr() {
    shaders_ = std::unordered_map<std::string, std::shared_ptr<Shader>>();
}

ShaderMgr::~ShaderMgr() {
    for (auto& shader : shaders_) {
        shader.second.reset();
    }
}

void ShaderMgr::Load(const std::string& name, const std::vector<char>& vertexSource, const std::vector<char>& fragSource) {
    shaders_[name] = std::make_shared<Shader>(vertexSource, fragSource);
}

} // namespace Vklib
