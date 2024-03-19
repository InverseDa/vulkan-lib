#include "swapchain.hpp"
#include "core/context.hpp"

namespace ida {

IdaSwapChain::IdaSwapChain(vk::Extent2D windowExtent) : windowExtent_(windowExtent) {
    Init();
}

IdaSwapChain::IdaSwapChain(vk::Extent2D windowExtent, std::shared_ptr<IdaSwapChain> previous)
    : windowExtent_(windowExtent), oldSwapChain_(previous) {
    Init();
    oldSwapChain_ = nullptr;
}

IdaSwapChain::~IdaSwapChain() {
    auto& device = Context::GetInstance().device;
    for (auto imageView : swapChainImageViews_) {
        device.destroyImageView(imageView);
    }
    swapChainImageViews_.clear();
    if (swapChain_ != nullptr) {
        device.destroySwapchainKHR(swapChain_);
    }

    for (size_t i = 0; i < depthImages_.size(); i++) {
        device.destroyImageView(depthImageViews_[i]);
        device.destroyImage(depthImages_[i]);
        device.freeMemory(depthImageMemories_[i]);
    }

    for (auto framebuffer : swapChainFramebuffers_) {
        device.destroyFramebuffer(framebuffer);
    }
    device.destroyRenderPass(renderPass_);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroySemaphore(imageAvailableSemaphores_[i]);
        device.destroySemaphore(renderFinishedSemaphores_[i]);
        device.destroyFence(inFlightFences_[i]);
    }
}

vk::Format IdaSwapChain::FindDepthFormat() {
    return vk::Format::eR16G16B16A16Uint;
}

vk::Result IdaSwapChain::AcquireNextImageIndex(uint32_t& imageIndex) {
    auto& device = Context::GetInstance().device;
    device.waitForFences(inFlightFences_[currentFrame], true, std::numeric_limits<std::uint64_t>::max());

    auto resultValue = device.acquireNextImageKHR(swapChain_, std::numeric_limits<std::uint64_t>::max(), imageAvailableSemaphores_[currentFrame], nullptr);
    if (resultValue.result != vk::Result::eSuccess) {
        return resultValue.result;
    }
    imageIndex = resultValue.value;
}

vk::Result IdaSwapChain::SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex) {
    return vk::Result::eErrorVideoProfileCodecNotSupportedKHR;
}

void IdaSwapChain::Init() {
    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateDepthResources();
    CreateFramebuffers();
    CreateSyncObjects();
}

void IdaSwapChain::CreateSwapChain() {
}

void IdaSwapChain::CreateImageViews() {
}

void IdaSwapChain::CreateDepthResources() {
}

void IdaSwapChain::CreateRenderPass() {
}

void IdaSwapChain::CreateFramebuffers() {
}

void IdaSwapChain::CreateSyncObjects() {
}

}

} // namespace Vklib