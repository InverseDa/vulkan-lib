#include "context.hpp"

namespace Vklib {
std::unique_ptr<Context> Context::instance_ = nullptr;

void Context::Init() {
    instance_.reset(new Context);
}

void Context::Quit() {
    instance_.reset();
}

Context::Context() {
    CreateInstance();
    PickupPhysicalDevice();
    QueryQueueFamilyIndices();
    CreateDevice();
    GetQueues();
}

Context::~Context() {
    device.destroy();
    instance.destroy();
}

void Context::CreateInstance() {
    vk::InstanceCreateInfo createInfo;
    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_VERSION_1_3);
    createInfo.setPApplicationInfo(&appInfo);
    instance = vk::createInstance(createInfo);

    std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};

    RemoveUnsupportedElems<const char*, vk::LayerProperties>(layers, vk::enumerateInstanceLayerProperties(), [](const char* e1, const vk::LayerProperties& e2) {
        return std::strcmp(e1, e2.layerName) == 0;
    });
    createInfo.setPEnabledLayerNames(layers);

    instance = vk::createInstance(createInfo);
}

void Context::PickupPhysicalDevice() {
    auto devices = instance.enumeratePhysicalDevices();
    // temporary find the first device
    phyDevice = devices[0];
    IO::PrintLog(LOG_LEVEL_INFO, "Physical Device: %s", phyDevice.getProperties().deviceName);
}

void Context::CreateDevice() {
    vk::DeviceCreateInfo createInfo;
    vk::DeviceQueueCreateInfo queueCreateInfo;
    float priorities = 1.0;
    queueCreateInfo.setPQueuePriorities(&priorities)
        .setQueueCount(1)
        .setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());
    createInfo.setQueueCreateInfos(queueCreateInfo);

    device = phyDevice.createDevice(createInfo);
}

void Context::QueryQueueFamilyIndices() {
    auto queueFamilies = phyDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < queueFamilies.size(); i++) {
        if (queueFamilies[i].queueFlags | vk::QueueFlagBits::eGraphics) {
            queueFamilyIndices.graphicsQueue = i;
            break;
        }
    }
}

void Context::GetQueues() {
    graphicsQueue = device.getQueue(queueFamilyIndices.graphicsQueue.value(), 0);
}

} // namespace Vklib