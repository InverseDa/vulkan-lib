#ifndef VULKAN_LIB_SWAPCHAIN_HPP
#define VULKAN_LIB_SWAPCHAIN_HPP

#include "vulkan/vulkan.hpp"

namespace Vklib {
class Swapchain final {
  public:
    vk::SwapchainKHR swapchain;

    Swapchain(int w, int h);
    ~Swapchain();

    struct SwapchainInfo {
        vk::Extent2D imageExtent;
        uint32_t imageCount;
        vk::SurfaceFormatKHR format;
        vk::SurfaceTransformFlagBitsKHR transform;
        vk::PresentModeKHR present;
    };

    SwapchainInfo info;
    std::vector<vk::Image> images;
    std::vector<vk::ImageView> imageViews;

    void QueryInfo(int w, int h);
    void GetImages();
    void CreateImageViews();
};
} // namespace Vklib

#endif // VULKAN_LIB_SWAPCHAIN_HPP
