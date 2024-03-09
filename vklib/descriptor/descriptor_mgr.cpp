#include "descriptor_mgr.hpp"
#include "vklib/core/context.hpp"

namespace Vklib {
std::unique_ptr<DescriptorSetMgr> DescriptorSetMgr::instance_ = nullptr;

DescriptorSetMgr::DescriptorSetMgr(uint32_t maxFlight) : maxFlight_(maxFlight) {
    vk::DescriptorPoolSize size;
    size
        .setType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(maxFlight * 2);
    vk::DescriptorPoolCreateInfo createInfo;
    createInfo
        .setMaxSets(maxFlight)
        .setPoolSizes(size);
    auto pool = Context::GetInstance().device.createDescriptorPool(createInfo);
    bufferSetPool_.pool_ = pool;
    bufferSetPool_.remainNum_ = maxFlight;
}

DescriptorSetMgr::~DescriptorSetMgr() {
    auto& device = Context::GetInstance().device;

    device.destroyDescriptorPool(bufferSetPool_.pool_);
    for (auto pool : fulledImageSetPool_) {
        device.destroyDescriptorPool(pool.pool_);
    }
    for (auto pool : availableImageSetPool_) {
        device.destroyDescriptorPool(pool.pool_);
    }
}

std::vector<DescriptorSetMgr::SetInfo> DescriptorSetMgr::AllocateBufferSets(uint32_t num) {
    std::vector<vk::DescriptorSetLayout> layouts(maxFlight_, Context::GetInstance().shader->GetDescriptorSetLayouts()[0]);
    vk::DescriptorSetAllocateInfo allocateInfo;
    allocateInfo
        .setDescriptorPool(bufferSetPool_.pool_)
        .setDescriptorSetCount(num)
        .setSetLayouts(layouts);
    auto sets = Context::GetInstance().device.allocateDescriptorSets(allocateInfo);

    std::vector<SetInfo> result(num);

    for (int i = 0; i < num; i++) {
        result[i].set = sets[i];
        result[i].pool = bufferSetPool_.pool_;
    }

    return result;
}

DescriptorSetMgr::SetInfo DescriptorSetMgr::AllocateImageSet() {
    std::vector<vk::DescriptorSetLayout> layouts(maxFlight_, Context::GetInstance().shader->GetDescriptorSetLayouts()[1]);
    vk::DescriptorSetAllocateInfo allocateInfo;
    auto& poolInfo = GetAvailableImagePoolInfo();
    allocateInfo
        .setDescriptorPool(poolInfo.pool_)
        .setDescriptorSetCount(1)
        .setSetLayouts(layouts);
    auto sets = Context::GetInstance().device.allocateDescriptorSets(allocateInfo);

    SetInfo result;
    result.pool = poolInfo.pool_;
    result.set = sets[0];

    poolInfo.remainNum_ = std::max<int>(static_cast<int>(poolInfo.remainNum_) - sets.size(), 0);
    if (poolInfo.remainNum_ == 0) {
        fulledImageSetPool_.push_back(poolInfo);
        availableImageSetPool_.pop_back();
    }

    return result;
}

void DescriptorSetMgr::FreeImageSet(const DescriptorSetMgr::SetInfo& info) {
    auto it = std::find_if(fulledImageSetPool_.begin(), fulledImageSetPool_.end(), [&](const PoolInfo& poolInfo) {
        return poolInfo.pool_ == info.pool;
    });
    if (it != fulledImageSetPool_.end()) {
        it->remainNum_++;
        availableImageSetPool_.push_back(*it);
        fulledImageSetPool_.erase(it);
        return;
    }

    it = std::find_if(availableImageSetPool_.begin(), availableImageSetPool_.end(), [&](const PoolInfo& poolInfo) {
        return poolInfo.pool_ == info.pool;
    });
    if (it != availableImageSetPool_.end()) {
        it->remainNum_++;
        return;
    }
    Context::GetInstance().device.freeDescriptorSets(info.pool, info.set);
}

void DescriptorSetMgr::AddImageSetPool() {
    constexpr uint32_t MaxSetNum = 100;

    vk::DescriptorPoolSize size;
    size
        .setType(vk::DescriptorType::eCombinedImageSampler)
        .setDescriptorCount(MaxSetNum);
    vk::DescriptorPoolCreateInfo createInfo;
    createInfo
        .setMaxSets(MaxSetNum)
        .setPoolSizes(size)
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
    auto pool = Context::GetInstance().device.createDescriptorPool(createInfo);
    availableImageSetPool_.push_back({pool, MaxSetNum});
}

DescriptorSetMgr::PoolInfo& DescriptorSetMgr::GetAvailableImagePoolInfo() {
    if (availableImageSetPool_.empty()) {
        AddImageSetPool();
    }
    return availableImageSetPool_.back();
}

} // namespace Vklib
