#include "swapchain.hpp"
#include "core/context.hpp"

namespace Vklib {
Swapchain::Swapchain(vk::SurfaceKHR surface, int windowWidth, int windowHeight) : surface(surface) {
    QuerySurfaceInfo(windowWidth, windowHeight);
    swapchain = CreateSwapchain();
    CreateImageAndViews();
}

Swapchain::~Swapchain() {
    auto& ctx = Context::GetInstance();
    for (auto& img : images) {
        ctx.device.destroyImageView(img.view);
    }
    for (auto& framebuffer : framebuffers) {
        ctx.device.destroyFramebuffer(framebuffer);
    }
    ctx.device.destroySwapchainKHR(swapchain);
    ctx.instance.destroySurfaceKHR(surface);
}

void Swapchain::InitFramebuffers() {
    CreateFramebuffers();
}

void Swapchain::QuerySurfaceInfo(int windowWidth, int windowHeight) {
    surfaceInfo_.format = QuerySurfaceFormat();

    auto capability = Context::GetInstance().phyDevice.getSurfaceCapabilitiesKHR(surface);
    surfaceInfo_.count = std::clamp(capability.minImageCount + 1, capability.minImageCount, capability.maxImageCount);
    surfaceInfo_.transform = capability.currentTransform;
    surfaceInfo_.extent = QuerySurfaceExtent(capability, windowWidth, windowHeight);
}

vk::SurfaceFormatKHR Swapchain::QuerySurfaceFormat() {
    auto formats = Context::GetInstance().phyDevice.getSurfaceFormatsKHR(surface);
    for (auto& format : formats) {
        if (format.format == vk::Format::eR8G8B8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format;
        }
    }
    return formats[0];
}

vk::Extent2D Swapchain::QuerySurfaceExtent(const vk::SurfaceCapabilitiesKHR& capability, int windowWidth, int windowHeight) {
    if (capability.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capability.currentExtent;
    } else {
        auto extent = vk::Extent2D{
            static_cast<uint32_t>(windowWidth),
            static_cast<uint32_t>(windowHeight)
        };

        extent.width = std::clamp(extent.width, capability.minImageExtent.width, capability.maxImageExtent.width);
        extent.height = std::clamp(extent.height, capability.minImageExtent.height, capability.maxImageExtent.height);
        return extent;
    }
}

vk::SwapchainKHR Swapchain::CreateSwapchain() {
    vk::SwapchainCreateInfoKHR createInfo;
    createInfo
        .setClipped(true)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setImageExtent(surfaceInfo_.extent)
        .setImageColorSpace(surfaceInfo_.format.colorSpace)
        .setImageFormat(surfaceInfo_.format.format)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setMinImageCount(surfaceInfo_.count)
        .setImageArrayLayers(1)
        .setPresentMode(vk::PresentModeKHR::eFifo)
        .setPreTransform(surfaceInfo_.transform)
        .setSurface(surface);

    auto& ctx = Context::GetInstance();
    if (ctx.queueInfo) {
        createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
    } else {
        createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
        std::array queueIndices = {ctx.queueInfo.graphicsIndex.value(), ctx.queueInfo.presentIndex.value()};
        createInfo.setQueueFamilyIndices(queueIndices);
    }

    return ctx.device.createSwapchainKHR(createInfo);
}

void Swapchain::CreateImageAndViews() {
    auto& ctx = Context::GetInstance();
    auto images = ctx.device.getSwapchainImagesKHR(swapchain);
    for (auto& image : images) {
        Image img;
        img.image = image;
        vk::ImageViewCreateInfo viewCreateInfo;
        vk::ImageSubresourceRange range;
        range
            .setBaseArrayLayer(0)
            .setBaseMipLevel(0)
            .setLayerCount(1)
            .setLevelCount(1)
            .setAspectMask(vk::ImageAspectFlagBits::eColor);
        viewCreateInfo
            .setImage(image)
            .setFormat(surfaceInfo_.format.format)
            .setViewType(vk::ImageViewType::e2D)
            .setSubresourceRange(range)
            .setComponents(vk::ComponentMapping{});
        img.view = ctx.device.createImageView(viewCreateInfo);
        this->images.push_back(img);
    }
}

void Swapchain::CreateFramebuffers() {
    for (auto& img : images) {
        auto& view = img.view;

        vk::FramebufferCreateInfo createInfo;
        createInfo
            .setAttachments(view)
            .setRenderPass(Context::GetInstance().renderProcess->renderPass)
            .setWidth(GetExtent().width)
            .setHeight(GetExtent().height)
            .setLayers(1);
        framebuffers.push_back(Context::GetInstance().device.createFramebuffer(createInfo));
    }
}

} // namespace Vklib