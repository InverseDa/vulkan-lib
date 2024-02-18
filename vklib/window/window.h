#ifndef WINDOW_H
#define WINDOW_H
#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"
#include "vkCore/vkBase.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <sstream>

class WindowWrapper {
  private:
    VkExtent2D m_size;
    std::string m_title;

    GLFWwindow* window;
    GLFWmonitor* monitor;

  public:
    WindowWrapper(VkExtent2D size, bool fullScreen = false, bool isResizable = true, bool limitFrameRate = true, std::string title = "Vulkan Application");
    ~WindowWrapper();
    // Judge if the window should be closed
    bool IsClose() const;
    // Make the window's context current
    void MakeContextCurrent() const;
    // Swap the front and back buffers of the window
    void SwapBuffers() const;
    // Poll for and process events
    void PollEvents() const;

    /**************** < Set Function And Method > ****************/
    void SetKeyCallback(GLFWkeyfun callback);
    void SetMouseButtonCallback(GLFWmousebuttonfun callback);
    void SetCursorPosCallback(GLFWcursorposfun callback);
    void SetScrollCallback(GLFWscrollfun callback);
    void SetFramebufferSizeCallback(GLFWframebuffersizefun callback);
    void SetWindowCloseCallback(GLFWwindowclosefun callback);
    void SetWindowRefreshCallback(GLFWwindowrefreshfun callback);
    void SetWindowFocusCallback(GLFWwindowfocusfun callback);
    void SetWindowIconifyCallback(GLFWwindowiconifyfun callback);
    void SetWindowMaximizeCallback(GLFWwindowmaximizefun callback);
    void SetWindowContentScaleCallback(GLFWwindowcontentscalefun callback);
    void SetWindowPosCallback(GLFWwindowposfun callback);
    void SetWindowSizeCallback(GLFWwindowsizefun callback);
    void SetWindowAspectRatioCallback(GLFWwindowsizefun callback);
    void SetInputMode(int mode, int value);
    /**************** < Set Function And Method > ****************/

    void ShowTitleFPS();

    /**************** < Get Function And Method > ****************/
    int GetWidth() const;
    int GetHeight() const;
    std::string GetTitle() const;
    /**************** < Get Function And Method > ****************/
};

#endif