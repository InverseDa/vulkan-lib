#include "vec2.hpp"
#include "vklib/mesh/vertex.hpp"

std::vector<vk::VertexInputAttributeDescription> Vec2::GetAttributeDescription() {
    std::vector<vk::VertexInputAttributeDescription> descriptions(2);
    descriptions[0]
        .setBinding(0)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setLocation(0)
        .setOffset(0);
    descriptions[1]
        .setBinding(0)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setLocation(1)
        .setOffset(offsetof(Vertex, texCoord));
    return descriptions;
}

std::vector<vk::VertexInputBindingDescription> Vec2::GetBindingDescription() {
    std::vector<vk::VertexInputBindingDescription> descriptions(1);
    descriptions[0]
        .setBinding(0)
        .setStride(sizeof(Vertex))
        .setInputRate(vk::VertexInputRate::eVertex);
    return descriptions;
}
