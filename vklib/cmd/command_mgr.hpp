#ifndef VULKAN_LIB_COMMAND_MGR_HPP
#define VULKAN_LIB_COMMAND_MGR_HPP

#include "vulkan/vulkan.hpp"
namespace Vklib {
class CommandMgr final {
  public:
    CommandMgr();
    ~CommandMgr();

    vk::CommandBuffer CreateACommandBuffer();
    std::vector<vk::CommandBuffer> CreateCommandBuffers(std::uint32_t count);
    void ResetCmds();
    void FreeCmd(const vk::CommandBuffer&);

  private:
    vk::CommandPool pool_;
    vk::CommandPool CreateCommandPool();
};
} // namespace Vklib

#endif // VULKAN_LIB_COMMAND_MGR_HPP
