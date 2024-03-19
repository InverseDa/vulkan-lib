#ifndef VULKAN_LIB_COMMAND_HPP
#define VULKAN_LIB_COMMAND_HPP

#include "vulkan/vulkan.hpp"
#include <functional>

namespace ida {

class CommandMgr {
  public:
    CommandMgr();
    ~CommandMgr();
    void ExecuteCommand(vk::Queue queue, std::function<void(vk::CommandBuffer&)> func);

  private:
    vk::CommandPool CreateCommandPool();

    std::vector<vk::CommandBuffer> CreateCommandBuffers(std::uint32_t count);
    void FreeCommand(const vk::CommandBuffer&);

    vk::CommandPool pool_;
};

} // namespace ida

#endif // VULKAN_LIB_COMMAND_HPP
