#ifndef VULKAN_LIB_WINDOW_HPP
#define VULKAN_LIB_WINDOW_HPP

#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include "vulkan/vulkan.hpp"
#include <string>

#include "tools.hpp"

namespace ida {
class IdaWindow {
  public:
    IdaWindow(int w, int h, std::string name);
    ~IdaWindow();
    IdaWindow(const IdaWindow&) = delete;
    IdaWindow& operator=(const IdaWindow&) = delete;

    bool ShouldClose() { return shouldClose_; }
    vk::Extent2D GetExtent() { return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)}; }
    SDL_Window* GetWindow() { return window_; }
    bool IsResizeNow() { return resizeNow_; }
    void ResetResizeFlag() { resizeNow_ = false; }

    void Run(std::function<void()> func);

    GetSurfaceCallback getSurfaceCallback;
    std::vector<const char*> extensions;

  private:
    void FrameBufferResizeCallback(SDL_Window* window, int width, int height);
    void InitWindow();

    std::string title_;
    int width_;
    int height_;

    SDL_Event event_;
    SDL_Window* window_;
    bool shouldClose_ = false;
    bool resizeNow_ = false;
};
} // namespace ida

#endif // VULKAN_LIB_WINDOW_HPP
