#include "buffer.hpp"
#include "vklib/core/context.hpp"

namespace Vklib {
Buffer::Buffer(vk::BufferUsageFlags usage, size_t size, vk::MemoryPropertyFlags memProperty) {
    auto& device = Context::GetInstance().device;

    this->size = size;
    vk::BufferCreateInfo createInfo;
    createInfo
        .setUsage(usage)
        .setSize(size)
        .setSharingMode(vk::SharingMode::eExclusive);

    buffer = device.createBuffer(createInfo);

    auto requirements = device.getBufferMemoryRequirements(buffer);
    requireSize = requirements.size;
    auto index = QueryBufferMemTypeIndex(requirements.memoryTypeBits, memProperty);
    vk::MemoryAllocateInfo allocateInfo;
    allocateInfo
        .setMemoryTypeIndex(index)
        .setAllocationSize(requirements.size);
    memory = device.allocateMemory(allocateInfo);

    device.bindBufferMemory(buffer, memory, 0);

    if (memProperty & vk::MemoryPropertyFlagBits::eHostVisible) {
        map = device.mapMemory(memory, 0, size);
    } else {
        map = nullptr;
    }
}

Buffer::~Buffer() {
    auto& device = Context::GetInstance().device;
    if (map) {
        device.unmapMemory(memory);
    }
    device.freeMemory(memory);
    device.destroyBuffer(buffer);
}

std::uint32_t Buffer::QueryBufferMemTypeIndex(std::uint32_t type, vk::MemoryPropertyFlags flag) {
    auto property = Context::GetInstance().phyDevice.getMemoryProperties();

    for (std::uint32_t i = 0; i < property.memoryTypeCount; i++) {
        if ((1 << i) & type &&
            property.memoryTypes[i].propertyFlags & flag) {
            return i;
        }
    }

    return 0;
}

} // namespace Vklib
