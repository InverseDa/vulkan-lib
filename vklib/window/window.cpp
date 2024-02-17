#include "window.h"

WindowWrapper::WindowWrapper(VkExtent2D size, bool fullScreen, bool isResizable, bool limitFrameRate, std::string title)
    : m_size(size),
      m_title(title) {
    if (!glfwInit()) {
        throw std::runtime_error("[WindowWrapper][ERROR] Failed to initialize GLFW!");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, isResizable);
    monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* pMode = glfwGetVideoMode(monitor);
    window = fullScreen ? glfwCreateWindow(size.width, size.height, title.c_str(), monitor, nullptr)
                        : glfwCreateWindow(size.width, size.height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("[WindowWrapper][ERROR] Failed to initialize GLFW!");
    }
    // Ready to initialize Vulkan
    // Push the required instance extensions
#ifdef _WIN32
    Vulkan::GraphicsBase::GetInstance().PushInstanceExtension("VK_KHR_surface");
    Vulkan::GraphicsBase::GetInstance().PushInstanceExtension("VK_KHR_win32_surface");
    Vulkan::GraphicsBase::GetInstance().PushDeviceExtension("VK_KHR_swapchain");
#else
    uint32_t extensionsCount = 0;
    const char** extensionNames;
    extensionNames = glfwGetRequiredInstanceExtensions(&extensionsCount);
    if (!extensionNames) {
        glfwTerminate();
        throw std::runtime_error("[WindowWrapper][ERROR] Failed to get required instance extensions!");
    }
    for (size_t i = 0; i < extensionsCount; i++) {
        Vulkan::GraphicsBase::GetInstance().PushInstanceExtension(extensionNames[i]);
    }
#endif
    // Now create the Vulkan instance
    Vulkan::GraphicsBase::GetInstance().UseLatestApiVersion();
    if (Vulkan::GraphicsBase::GetInstance().CreateInstance()) {
        throw std::runtime_error("[WindowWrapper][ERROR] Failed to create Vulkan instance!");
    }
    // Create the window surface
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (VkResult result = glfwCreateWindowSurface(
            Vulkan::GraphicsBase::GetInstance().GetInstanceHandle(), window, nullptr, &surface)) {
        glfwTerminate();
        throw std::runtime_error("[WindowWrapper][ERROR] Failed to create window surface!");
    }
    Vulkan::GraphicsBase::GetInstance().Surface(surface);
    // Get the physical devices
    if (Vulkan::GraphicsBase::GetInstance().GetPhysicalDevices() ||
        Vulkan::GraphicsBase::GetInstance().DeterminePhysicalDevice(0, true, false) ||
        Vulkan::GraphicsBase::GetInstance().CreateDevice()) {
        throw std::runtime_error("[WindowWrapper][ERROR] Failed to get physical devices!");
    }
    // Get the swap chain
    if (Vulkan::GraphicsBase::GetInstance().CreateSwapChain(limitFrameRate)) {
        throw std::runtime_error("[WindowWrapper][ERROR] Failed to create swap chain!");
    }
}

WindowWrapper::~WindowWrapper() {
    Vulkan::GraphicsBase::GetInstance().WaitDeviceIdle();
    glfwTerminate();
}

void WindowWrapper::ShowTitleFPS() {
    static double time0 = glfwGetTime();
    static double time1;
    static double dt;
    static int dframe = -1;
    static std::stringstream info;
    time1 = glfwGetTime();
    dframe++;
    if ((dt = time1 - time0) >= 1) {
        info.precision(1);
        info << m_title << "    " << std::fixed << dframe / dt << " FPS";
        glfwSetWindowTitle(window, info.str().c_str());
        info.str("");
        time0 = time1;
        dframe = 0;
    }
}

bool WindowWrapper::IsClose() const {
    return glfwWindowShouldClose(window);
}

void WindowWrapper::MakeContextCurrent() const {
    glfwMakeContextCurrent(window);
}

void WindowWrapper::SetKeyCallback(GLFWkeyfun callback) {
    glfwSetKeyCallback(window, callback);
}

void WindowWrapper::SetMouseButtonCallback(GLFWmousebuttonfun callback) {
    glfwSetMouseButtonCallback(window, callback);
}

void WindowWrapper::SetCursorPosCallback(GLFWcursorposfun callback) {
    glfwSetCursorPosCallback(window, callback);
}

void WindowWrapper::SetScrollCallback(GLFWscrollfun callback) {
    glfwSetScrollCallback(window, callback);
}

void WindowWrapper::SetFramebufferSizeCallback(GLFWframebuffersizefun callback) {
    glfwSetFramebufferSizeCallback(window, callback);
}

void WindowWrapper::SetWindowCloseCallback(GLFWwindowclosefun callback) {
    glfwSetWindowCloseCallback(window, callback);
}

void WindowWrapper::SetWindowRefreshCallback(GLFWwindowrefreshfun callback) {
    glfwSetWindowRefreshCallback(window, callback);
}

void WindowWrapper::SetWindowFocusCallback(GLFWwindowfocusfun callback) {
    glfwSetWindowFocusCallback(window, callback);
}

void WindowWrapper::SetWindowIconifyCallback(GLFWwindowiconifyfun callback) {
    glfwSetWindowIconifyCallback(window, callback);
}

void WindowWrapper::SetWindowMaximizeCallback(GLFWwindowmaximizefun callback) {
    glfwSetWindowMaximizeCallback(window, callback);
}

void WindowWrapper::SetWindowContentScaleCallback(GLFWwindowcontentscalefun callback) {
    glfwSetWindowContentScaleCallback(window, callback);
}

void WindowWrapper::SetWindowPosCallback(GLFWwindowposfun callback) {
    glfwSetWindowPosCallback(window, callback);
}

void WindowWrapper::SetWindowSizeCallback(GLFWwindowsizefun callback) {
    glfwSetWindowSizeCallback(window, callback);
}

void WindowWrapper::SetWindowAspectRatioCallback(GLFWwindowsizefun callback) {
    glfwSetWindowSizeCallback(window, callback);
}

void WindowWrapper::SetInputMode(int mode, int value) {
    glfwSetInputMode(window, mode, value);
}

int WindowWrapper::GetWidth() const {
    return m_size.width;
}

int WindowWrapper::GetHeight() const {
    return m_size.height;
}

std::string WindowWrapper::GetTitle() const {
    return m_title;
}

void WindowWrapper::SwapBuffers() const {
    glfwSwapBuffers(window);
}

void WindowWrapper::PollEvents() const {
    glfwPollEvents();
}