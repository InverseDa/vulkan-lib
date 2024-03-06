#ifndef VULKAN_LIB_SWAPCHAIN_HPP
#define VULKAN_LIB_SWAPCHAIN_HPP

#include "vulkan/vulkan.hpp"

namespace Vklib {
class Swapchain final {
  public:
    struct Image {
        vk::Image image;
        vk::ImageView view;
    };

    vk::SurfaceKHR surface = nullptr;
    vk::SwapchainKHR swapchain = nullptr;
    std::vector<Image> images;
    std::vector<vk::Framebuffer> framebuffers;

    const auto& GetExtent() const { return surfaceInfo_.extent; }
    const auto& GetFormat() const { return surfaceInfo_.format; }

    Swapchain(vk::SurfaceKHR, int windowWidth, int windowHeight);
    ~Swapchain();

    void InitFramebuffers();

  private:
    struct SwapchainInfo {
        vk::SurfaceFormatKHR format;
        vk::Extent2D extent;
        std::uint32_t count;
        vk::SurfaceTransformFlagBitsKHR transform;
    } surfaceInfo_;

    vk::SwapchainKHR CreateSwapchain();

    void QuerySurfaceInfo(int windowWidth, int windowHeight);
    vk::SurfaceFormatKHR QuerySurfaceFormat();
    vk::Extent2D QuerySurfaceExtent(const vk::SurfaceCapabilitiesKHR& capability, int windowWidth, int windowHeight);
    void CreateImageAndViews();
    void CreateFramebuffers();
};
} // namespace Vklib

#endif // VULKAN_LIB_SWAPCHAIN_HPP
