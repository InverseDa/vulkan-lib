#ifndef VULKAN_LIB_BUFFER_HPP
#define VULKAN_LIB_BUFFER_HPP

#include "vulkan/vulkan.hpp"

namespace Vklib {
class Buffer final {
  public:
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    void* map;
    size_t size;
    size_t requireSize;

    Buffer(vk::BufferUsageFlags usage, size_t size, vk::MemoryPropertyFlags memProperty);
    ~Buffer();

  private:

    std::uint32_t QueryBufferMemTypeIndex(std::uint32_t requirementBit, vk::MemoryPropertyFlags);
};
} // namespace Vklib

#endif // VULKAN_LIB_BUFFER_HPP
