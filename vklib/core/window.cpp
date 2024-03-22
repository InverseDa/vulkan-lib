#include "window.hpp"
#include "log/log.hpp"
#include "context.hpp"

namespace ida {

IdaWindow::IdaWindow(int w, int h, std::string name) : width_(w), height_(h), title_(name) {
    InitWindow();
}

IdaWindow::~IdaWindow() {
    SDL_Quit();
    SDL_DestroyWindow(window_);
}

void IdaWindow::InitWindow() {
    SDL_Init(SDL_INIT_EVERYTHING);
    window_ = SDL_CreateWindow(title_.c_str(),
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               width_,
                               height_,
                               SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (window_ == nullptr) {
        SDL_Log("Create window failed, ERROR MSG: %s", SDL_GetError());
        IO::ThrowError("SDL Can't create window");
    }
    unsigned int count;
    SDL_Vulkan_GetInstanceExtensions(window_, &count, nullptr);
    this->extensions.resize(count);
    SDL_Vulkan_GetInstanceExtensions(window_, &count, extensions.data());

    getSurfaceCallback = [&](vk::Instance instance) {
        VkSurfaceKHR surface;
        if (!SDL_Vulkan_CreateSurface(window_, instance, &surface)) {
            IO::ThrowError("SDL Can't create surface");
        }
        return surface; };
}

void IdaWindow::Run(std::function<void()> func) {
    while (!shouldClose_) {
        while (SDL_PollEvent(&event_)) {
            if (event_.type == SDL_QUIT) {
                shouldClose_ = true;
            }
            if (event_.type == SDL_WINDOWEVENT && event_.window.event == SDL_WINDOWEVENT_RESIZED) {
                width_ = event_.window.data1;
                height_ = event_.window.data2;
                FrameBufferResizeCallback(window_, width_, height_);
            }
        }
        func();
    }
}

void IdaWindow::FrameBufferResizeCallback(SDL_Window* window, int width, int height) {
    auto idaWindow = reinterpret_cast<IdaWindow*>(SDL_GetWindowData(window, "IdaWindow"));
    idaWindow->resizeNow_ = true;
    idaWindow->width_ = width;
    idaWindow->height_ = height;
}

} // namespace ida