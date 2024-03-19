#include "context.hpp"
#include "log/log.hpp"
#include "tools.hpp"

namespace ida {
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
    IO::PrintLog(LOG_LEVEL_INFO, "Physical device name: {}", phyDevice.getProperties().deviceName.data());

    GetSurface();

    device = CreateDevice(surface_);
    if (!device) {
        IO::ThrowError("Failed to create device");
    }

    graphicsQueue = device.getQueue(queueInfo.graphicsIndex.value(), 0);
    presentQueue = device.getQueue(queueInfo.presentIndex.value(), 0);
}

Context::~Context() {
//    ShaderMgr::Quit();
    device.destroySampler(sampler);
    commandMgr.reset();
    renderProcess.reset();
    swapChain.reset();
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
//    renderProcess = std::make_unique<RenderProcess>();
}

void Context::InitSwapChain(int windowWidth, int windowHeight) {
    swapChain = std::make_unique<IdaSwapChain>(surface_, windowWidth, windowHeight);
}

void Context::InitGraphicsPipeline() {
//    renderProcess->CreateGraphicsPipeline(*ShaderMgr::GetInstance().Get("default"));
}

void Context::InitCommandPool() {
    commandMgr = std::make_unique<CommandMgr>();
}

void Context::InitShaderModules() {
    auto vertexSource = ReadWholeFile(GetTestsPath("shaderMgrTest/shaders/frag.spv"));
    auto fragSource = ReadWholeFile(GetTestsPath("shaderMgrTest/shaders/vert.spv"));
}

void Context::InitSampler() {
    auto createInfo = vk::SamplerCreateInfo()
                          .setMagFilter(vk::Filter::eLinear)
                          .setMinFilter(vk::Filter::eLinear)
                          .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                          .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                          .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                          .setAnisotropyEnable(false)
                          .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                          .setUnnormalizedCoordinates(false)
                          .setCompareEnable(false)
                          .setMipmapMode(vk::SamplerMipmapMode::eLinear);
    sampler = Context::GetInstance().device.createSampler(createInfo);
}

void Context::GetSurface() {
    surface_ = getSurfaceCb_(instance);
    if (!surface_) {
        IO::ThrowError("Failed to create surface");
    }
}

void Context::InitVulkan(int windowWidth, int windowHeight) {
    InitSwapChain(windowWidth, windowHeight);
    InitShaderModules();
    InitRenderProcess();
    InitGraphicsPipeline();
    swapChain->InitFramebuffers();
    InitCommandPool();
    InitSampler();
}

} // namespace ida