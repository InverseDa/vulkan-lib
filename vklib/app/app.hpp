#ifndef VULKAN_LIB_APP_HPP
#define VULKAN_LIB_APP_HPP

#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include "vulkan/vulkan.hpp"
#include <memory>
#include <vector>

#include "log/log.hpp"
#include "render/renderer.hpp"
#include "core/context.hpp"

class Application {
  public:
    friend class Vklib::Context;
    static Application* GetInstance(const std::string& title, float width, float height) {
        if (!instance_) {
            instance_.reset(new Application(title, width, height));
        }
        return instance_.get();
    }

    ~Application();

    int Run();

  private:
    static std::unique_ptr<Application> instance_;
    std::unique_ptr<Vklib::Renderer> renderer_;

    std::string title_;
    float width_;
    float height_;

    SDL_Event event_;
    SDL_Window* window_;
    bool shouldClose = false;

    Application(const std::string& title, float width, float height);
    void InitVulkan(std::vector<const char*>& extensions, Vklib::Context::GetSurfaceCallback cb, int windowWidth, int windowHeight);
    void DestroyVulkan();
};

#endif // VULKAN_LIB_APP_HPP
