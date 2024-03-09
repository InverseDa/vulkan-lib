#ifndef VULKAN_LIB_DESCRIPTOR_MGR_HPP
#define VULKAN_LIB_DESCRIPTOR_MGR_HPP

#include "vulkan/vulkan.hpp"
#include <vector>
#include <memory>

namespace Vklib {
class DescriptorSetMgr final {
  public:
    struct SetInfo {
        vk::DescriptorSet set;
        vk::DescriptorPool pool;
    };

    static void Init(uint32_t maxFlight) {
        instance_.reset(new DescriptorSetMgr(maxFlight));
    }

    static void Quit() {
        instance_.reset();
    }

    static DescriptorSetMgr& GetInstance() {
        return *instance_;
    }

    DescriptorSetMgr(uint32_t maxFlight);
    ~DescriptorSetMgr();

    std::vector<SetInfo> AllocateBufferSets(uint32_t num);
    SetInfo AllocateImageSet();

    void FreeImageSet(const SetInfo&);

  private:
    struct PoolInfo {
        vk::DescriptorPool pool_;
        uint32_t remainNum_;
    };

    PoolInfo bufferSetPool_;

    std::vector<PoolInfo> fulledImageSetPool_;
    std::vector<PoolInfo> availableImageSetPool_;

    void AddImageSetPool();
    PoolInfo& GetAvailableImagePoolInfo();

    uint32_t maxFlight_;

    static std::unique_ptr<DescriptorSetMgr> instance_;
};
} // namespace Vklib

#endif // VULKAN_LIB_DESCRIPTOR_MGR_HPP
