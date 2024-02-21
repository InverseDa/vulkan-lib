#ifndef VULKAN_LIB_VKSEMAPHORE_H
#define VULKAN_LIB_VKSEMAPHORE_H

#include "vulkan/vulkan.h"
#include "macro.h"
#include "type/vkResult.h"
#include "core/ostream.h"

namespace Vulkan {
class Semaphore {
  public:
    Semaphore(VkSemaphoreCreateInfo& createInfo);
    Semaphore();
    Semaphore(Semaphore&& other) noexcept;
    ~Semaphore();

    // Getter
    DefineHandleTypeOperator;
    DefineAddressFunction;

    ResultType Create();
    ResultType Create(VkSemaphoreCreateInfo& createInfo);

  private:
    VkSemaphore handle = VK_NULL_HANDLE;
};
} // namespace Vulkan

#endif // VULKAN_LIB_VKSEMAPHORE_H
