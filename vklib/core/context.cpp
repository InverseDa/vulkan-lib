#include "context.hpp"
#include "log/log.hpp"

namespace Vklib {
std::unique_ptr<Context> Context::instance_ = nullptr;

void Context::Init(const std::vector<const char*>& extensions, CreateSurfaceFunc func) {
    instance_.reset(new Context(extensions, func));
}

void Context::Quit() {
    instance_.reset();
}

Context::Context(const std::vector<const char*>& extensions, CreateSurfaceFunc func) {
    CreateInstance(extensions);
    PickupPhysicalDevice();
    surface = func(instance);
    QueryQueueFamilyIndices();
    CreateDevice();
    GetQueues();
    renderProcess.reset(new RenderProcess);
}

Context::~Context() {
    instance.destroySurfaceKHR(surface);
    device.destroy();
    instance.destroy();
}

void Context::CreateInstance(const std::vector<const char*>& extensions) {
    vk::InstanceCreateInfo createInfo;
    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_VERSION_1_3);

    createInfo.setPApplicationInfo(&appInfo);
    instance = vk::createInstance(createInfo);

    std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};

    RemoveUnsupportedElems<const char*, vk::LayerProperties>(layers, vk::enumerateInstanceLayerProperties(), [](const char* e1, const vk::LayerProperties& e2) {
        return std::strcmp(e1, e2.layerName) == 0;
    });
    createInfo
        .setPEnabledLayerNames(layers)
        .setPEnabledExtensionNames(extensions);

    instance = vk::createInstance(createInfo);
}

void Context::PickupPhysicalDevice() {
    auto devices = instance.enumeratePhysicalDevices();
    // temporary find the first device
    phyDevice = devices[0];
    IO::PrintLog(LOG_LEVEL_INFO, "Physical Device: {}", phyDevice.getProperties().deviceName.data());
}

void Context::CreateDevice() {
    std::array extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    vk::DeviceCreateInfo createInfo;
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    float priorities = 1.0;
    if (queueFamilyIndices.presentQueue.value() == queueFamilyIndices.graphicsQueue.value()) {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo
            .setPQueuePriorities(&priorities)
            .setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value())
            .setQueueCount(1);
        queueCreateInfos.push_back(std::move(queueCreateInfo));
    } else {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo
            .setPQueuePriorities(&priorities)
            .setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value())
            .setQueueCount(1);
        queueCreateInfos.push_back(std::move(queueCreateInfo));
        queueCreateInfo
            .setPQueuePriorities(&priorities)
            .setQueueFamilyIndex(queueFamilyIndices.presentQueue.value())
            .setQueueCount(1);
        queueCreateInfos.push_back(std::move(queueCreateInfo));
    }
    createInfo
        .setQueueCreateInfos(queueCreateInfos)
        .setPEnabledExtensionNames(extensions);

    device = phyDevice.createDevice(createInfo);
}

void Context::QueryQueueFamilyIndices() {
    auto queueFamilies = phyDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < queueFamilies.size(); i++) {
        const auto& queueFamily = queueFamilies[i];
        if (queueFamily.queueFlags | vk::QueueFlagBits::eGraphics) {
            queueFamilyIndices.graphicsQueue = i;
        }
        if (phyDevice.getSurfaceSupportKHR(i, surface)) {
            queueFamilyIndices.presentQueue = i;
        }
        if (queueFamilyIndices) {
            break;
        }
    }
}

void Context::GetQueues() {
    graphicsQueue = device.getQueue(queueFamilyIndices.graphicsQueue.value(), 0);
    presentQueue = device.getQueue(queueFamilyIndices.presentQueue.value(), 0);
}

} // namespace Vklib