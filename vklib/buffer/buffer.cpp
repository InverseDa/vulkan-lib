#include "buffer.hpp"
#include "vklib/core/context.hpp"

namespace Vklib {
Buffer::Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property)
    : size(size) {
    CreateBuffer(size, usage);
    MemoryInfo info = QueryMemoryInfo(property);
    AllocateMemory(info);
    BindingMem2Buf();
}

Buffer::~Buffer() {
    Context::GetInstance().device.freeMemory(memory);
    Context::GetInstance().device.destroyBuffer(buffer);
}

void Buffer::CreateBuffer(size_t size, vk::BufferUsageFlags usage) {
    vk::BufferCreateInfo createInfo;
    createInfo
        .setSize(size)
        .setUsage(usage)
        .setSharingMode(vk::SharingMode::eExclusive);

    buffer = Context::GetInstance().device.createBuffer(createInfo);
}

void Buffer::AllocateMemory(MemoryInfo info) {
    vk::MemoryAllocateInfo allocateInfo;
    allocateInfo
        .setMemoryTypeIndex(info.index)
        .setAllocationSize(info.size);
    memory = Context::GetInstance().device.allocateMemory(allocateInfo);
}

Buffer::MemoryInfo Buffer::QueryMemoryInfo(vk::MemoryPropertyFlags property) {
    MemoryInfo info;
    vk::MemoryRequirements requirements = Context::GetInstance().device.getBufferMemoryRequirements(buffer);
    info.size = requirements.size;

    auto properties = Context::GetInstance().phyDevice.getMemoryProperties();
    for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
        if ((1 << i) & requirements.memoryTypeBits &&
            properties.memoryTypes[i].propertyFlags & property) {
            info.index = i;
            break;
        }
    }

    return info;
}

void Buffer::BindingMem2Buf() {
    Context::GetInstance().device.bindBufferMemory(buffer, memory, 0);
}

} // namespace Vklib
