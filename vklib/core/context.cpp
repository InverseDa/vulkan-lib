#include "context.hpp"
#include "log/log.hpp"

namespace Vklib {
Context* Context::instance_ = nullptr;

void Context::Init(std::vector<const char*>& extensions, GetSurfaceCallback cb) {
    instance_ = new Context(extensions, cb);
}

void Context::Quit() {
    delete instance_;
}

Context& Context::GetInstance() {
    return *instance_;
}

Context::Context(std::vector<const char*>& extensions, GetSurfaceCallback cb) {
    getSurfaceCb_ = cb;

    instance = CreateInstance(extensions);
    if (!instance) {
        IO::ThrowError("Failed to create instance");
    }

    phyDevice = PickupPhysicalDevice();
    if (!phyDevice) {
        IO::ThrowError("Failed to pickup physical device");
    }

    surface_ = getSurfaceCb_(instance);
    if (!surface_) {
        IO::ThrowError("Failed to create surface");
    }

    device = CreateDevice(surface_);
    if (!device) {
        IO::ThrowError("Failed to create device");
    }

    graphicsQueue = device.getQueue(queueInfo.graphicsIndex.value(), 0);
    presentQueue = device.getQueue(queueInfo.presentIndex.value(), 0);
}

Context::~Context() {
    commandMgr.reset();
    renderProcess.reset();
    swapchain.reset();
    device.destroy();
    instance.destroy();
}

vk::Instance Context::CreateInstance(std::vector<const char*>& extensions) {
    vk::InstanceCreateInfo createInfo;
    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_VERSION_1_3);
    createInfo
        .setPApplicationInfo(&appInfo)
        .setPEnabledExtensionNames(extensions);

    std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
    createInfo.setPEnabledLayerNames(layers);

    return vk::createInstance(createInfo);
}

vk::PhysicalDevice Context::PickupPhysicalDevice() {
    auto devices = instance.enumeratePhysicalDevices();
    if (devices.size() == 0) {
        IO::ThrowError("Failed to find physical devices suitable for vulkan! Make sure you have a compatible GPU installed.");
    }
    return devices[0];
}

vk::Device Context::CreateDevice(vk::SurfaceKHR surface) {
    vk::DeviceCreateInfo deviceCreateInfo;
    QueryQueueInfo(surface);
    std::array extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    deviceCreateInfo.setPEnabledExtensionNames(extensions);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    float priority = 1.0;
    if (queueInfo.graphicsIndex.value() == queueInfo.presentIndex.value()) {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.setPQueuePriorities(&priority);
        queueCreateInfo.setQueueFamilyIndex(queueInfo.graphicsIndex.value());
        queueCreateInfo.setQueueCount(1);
        queueCreateInfos.push_back(queueCreateInfo);
    } else {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.setPQueuePriorities(&priority);
        queueCreateInfo.setQueueFamilyIndex(queueInfo.graphicsIndex.value());
        queueCreateInfo.setQueueCount(1);
        queueCreateInfos.push_back(queueCreateInfo);

        queueCreateInfo.setQueueFamilyIndex(queueInfo.presentIndex.value());
        queueCreateInfos.push_back(queueCreateInfo);
    }
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfos);

    return phyDevice.createDevice(deviceCreateInfo);
}

void Context::QueryQueueInfo(vk::SurfaceKHR surface) {
    auto queueFamilies = phyDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < queueFamilies.size(); i++) {
        if (queueFamilies[i].queueFlags | vk::QueueFlagBits::eGraphics) {
            queueInfo.graphicsIndex = i;
        }
        if (phyDevice.getSurfaceSupportKHR(i, surface)) {
            queueInfo.presentIndex = i;
        }
        if (queueInfo) {
            break;
        }
    }
}

void Context::InitRenderProcess() {
     renderProcess = std::make_unique<RenderProcess>();
}

void Context::InitSwapchain(int windowWidth, int windowHeight) {
    swapchain = std::make_unique<Swapchain>(surface_, windowWidth, windowHeight);
}

void Context::InitGraphicsPipeline() {
    auto vertexSource = ReadWholeFile("shaders/vert.spv");
    auto fragSource = ReadWholeFile("shaders/frag.spv");
    renderProcess->RecreateGraphicsPipeline(vertexSource, fragSource);
}

void Context::InitCommandPool() {
    commandMgr = std::make_unique<CommandMgr>();
}

} // namespace Vklib