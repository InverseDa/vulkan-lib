#ifndef VULKAN_LIB_VKDESCRIPTOR_H
#define VULKAN_LIB_VKDESCRIPTOR_H

#include "macro.h"
#include "vulkan/vulkan.h"

namespace Vulkan {
class DescriptorSetLayout {
  public:
    DescriptorSetLayout() = default;
    DescriptorSetLayout(VkDescriptorSetLayoutCreateInfo& createInfo);
    DescriptorSetLayout(DescriptorSetLayout&&);
    ~DescriptorSetLayout();

    // Getter
    DefineHandleTypeOperator;
    DefineAddressFunction;

    ResultType Create(VkDescriptorSetLayoutCreateInfo& createInfo);

  private:
    VkDescriptorSetLayout handle = VK_NULL_HANDLE;
};

class DescriptorSet {
  public:
  private:
    friend class DescriptorPool;
};

class DescriptorPool {
  public:
  private:
};
} // namespace Vulkan

#endif // VULKAN_LIB_VKDESCRIPTOR_H
