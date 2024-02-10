#include "window.h"

// Path: src/glfw.cpp
Init::Init() {
    if (!glfwInit()) { throw std::runtime_error("Failed to initialize GLFW."); }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

Init& Init::GetInstance() {
    static Init instance;
    return instance;
}

Init::~Init() {
    glfwTerminate();
}

WindowWrapper::WindowWrapper(int width, int height, std::string& title)
    : m_width(width),
      m_height(height),
      m_title(title),
      window(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr),
             [](GLFWwindow* window) { glfwDestroyWindow(window); }) {
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window.");
    }

    WindowWrapper::MakeContextCurrent();
}

std::shared_ptr<WindowWrapper> WindowWrapper::CreateWindow(int width, int height, std::string&& title) {
    Init::GetInstance();
    auto wrapper = std::make_shared<WindowWrapper>(width, height, std::move(title));
    return wrapper;
}

WindowWrapper::~WindowWrapper() { // TODO: 关闭窗口
}

GLFWwindow* WindowWrapper::GetRawPtr() const {
    return window.get();
}

bool WindowWrapper::IsClose() const {
    return glfwWindowShouldClose(window.get());
}

void WindowWrapper::MakeContextCurrent() const {
    glfwMakeContextCurrent(window.get());
}

void WindowWrapper::SetKeyCallback(GLFWkeyfun callback) {
    glfwSetKeyCallback(window.get(), callback);
}

void WindowWrapper::SetMouseButtonCallback(GLFWmousebuttonfun callback) {
    glfwSetMouseButtonCallback(window.get(), callback);
}

void WindowWrapper::SetCursorPosCallback(GLFWcursorposfun callback) {
    glfwSetCursorPosCallback(window.get(), callback);
}

void WindowWrapper::SetScrollCallback(GLFWscrollfun callback) {
    glfwSetScrollCallback(window.get(), callback);
}

void WindowWrapper::SetFramebufferSizeCallback(GLFWframebuffersizefun callback) {
    glfwSetFramebufferSizeCallback(window.get(), callback);
}

void WindowWrapper::SetWindowCloseCallback(GLFWwindowclosefun callback) {
    glfwSetWindowCloseCallback(window.get(), callback);
}

void WindowWrapper::SetWindowRefreshCallback(GLFWwindowrefreshfun callback) {
    glfwSetWindowRefreshCallback(window.get(), callback);
}

void WindowWrapper::SetWindowFocusCallback(GLFWwindowfocusfun callback) {
    glfwSetWindowFocusCallback(window.get(), callback);
}

void WindowWrapper::SetWindowIconifyCallback(GLFWwindowiconifyfun callback) {
    glfwSetWindowIconifyCallback(window.get(), callback);
}

void WindowWrapper::SetWindowMaximizeCallback(GLFWwindowmaximizefun callback) {
    glfwSetWindowMaximizeCallback(window.get(), callback);
}

void WindowWrapper::SetWindowContentScaleCallback(GLFWwindowcontentscalefun callback) {
    glfwSetWindowContentScaleCallback(window.get(), callback);
}

void WindowWrapper::SetWindowPosCallback(GLFWwindowposfun callback) {
    glfwSetWindowPosCallback(window.get(), callback);
}

void WindowWrapper::SetWindowSizeCallback(GLFWwindowsizefun callback) {
    glfwSetWindowSizeCallback(window.get(), callback);
}

void WindowWrapper::SetWindowAspectRatioCallback(GLFWwindowsizefun callback) {
    glfwSetWindowSizeCallback(window.get(), callback);
}

void WindowWrapper::SetInputMode(int mode, int value) {
    glfwSetInputMode(window.get(), mode, value);
}

int WindowWrapper::GetWidth() const {
    return m_width;
}

int WindowWrapper::GetHeight() const {
    return m_height;
}

std::string WindowWrapper::GetTitle() const {
    return m_title;
}

void WindowWrapper::SwapBuffers() const {
    glfwSwapBuffers(window.get());
}

void WindowWrapper::PollEvents() const {
    glfwPollEvents();
}