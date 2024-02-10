#pragma once
#include "GLFW/glfw3.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

class Init {
  private:
    Init();

  public:
    static Init& GetInstance();
    ~Init();
};

class WindowWrapper {
  private:
    int m_width;
    int m_height;
    std::string m_title;

    std::shared_ptr<GLFWwindow> window;

  public:
    WindowWrapper(int width, int height, std::string& title);
    ~WindowWrapper();
    // Create a new window
    static std::shared_ptr<WindowWrapper>
    CreateWindow(int width = 800, int height = 600, std::string&& title = "GLFW Window");
    // Get the raw pointer to the GLFWwindow
    GLFWwindow* GetRawPtr() const;
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

    /**************** < Get Function And Method > ****************/
    int GetWidth() const;
    int GetHeight() const;
    std::string GetType() const;
    std::string GetTitle() const;
    /**************** < Get Function And Method > ****************/
};