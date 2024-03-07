#include "vec2.hpp"

vk::VertexInputAttributeDescription Vec2::GetAttributeDescription() {
    vk::VertexInputAttributeDescription description;
    description.setBinding(0)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setLocation(0)
        .setOffset(0);
    return description;
}

vk::VertexInputBindingDescription Vec2::GetBindingDescription() {
    vk::VertexInputBindingDescription description;
    description.setBinding(0)
        .setStride(sizeof(float) * 2)
        .setInputRate(vk::VertexInputRate::eVertex);
    return description;
}

