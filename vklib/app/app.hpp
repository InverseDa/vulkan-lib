#ifndef VULKAN_LIB_APP_HPP
#define VULKAN_LIB_APP_HPP

#include "vulkan/vulkan.hpp"
#include <memory>
#include <vector>

#include "log/log.hpp"
#include "render/renderer.hpp"
#include "core/context.hpp"
#include "descriptor/descriptors.hpp"
#include "core/window.hpp"

class Application {
  public:
    static Application* GetInstance(const std::string& title, int width, int height) {
        if (!instance_) {
            instance_.reset(new Application(title, width, height));
        }
        return instance_.get();
    }

    ~Application();

    int Run();

  private:
    static std::unique_ptr<Application> instance_;
    std::unique_ptr<ida::IdaWindow> window_;
    std::unique_ptr<ida::IdaRenderer> renderer_;
    std::unique_ptr<ida::IdaDescriptorPool> globalPool{};

    Application(const std::string& title, int width, int height);
    void InitVulkan(std::vector<const char*>& extensions, GetSurfaceCallback cb, int windowWidth, int windowHeight);
    void DestroyVulkan();
};

#endif // VULKAN_LIB_APP_HPP
