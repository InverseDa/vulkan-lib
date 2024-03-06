#ifndef VULKAN_LIB_BUFFER_HPP
#define VULKAN_LIB_BUFFER_HPP

#include "vulkan/vulkan.hpp"

namespace Vklib {
class Buffer final {
  public:
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    size_t size;

    Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property);
    ~Buffer();

  private:
    struct MemoryInfo final {
        size_t size;
        uint32_t index;
    };

    void CreateBuffer(size_t size, vk::BufferUsageFlags usage);
    void AllocateMemory(MemoryInfo info);
    void BindingMem2Buf();
    MemoryInfo QueryMemoryInfo(vk::MemoryPropertyFlags property);
};
} // namespace Vklib

#endif // VULKAN_LIB_BUFFER_HPP
