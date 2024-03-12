#include "app.hpp"

std::unique_ptr<Application> Application::instance_ = nullptr;

Application::Application(const std::string& title, float width, float height) : title_(title), width_(width), height_(height) {
    SDL_Init(SDL_INIT_EVERYTHING);
    window_ = SDL_CreateWindow(title_.c_str(),
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               width,
                               height,
                               SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (window_ == nullptr) {
        SDL_Log("Create window failed, ERROR MSG: %s", SDL_GetError());
        IO::ThrowError("SDL Can't create window");
    }
    unsigned int count;
    SDL_Vulkan_GetInstanceExtensions(window_, &count, nullptr);
    std::vector<const char*> extensions(count);
    SDL_Vulkan_GetInstanceExtensions(window_, &count, extensions.data());

    InitVulkan(extensions, [&](vk::Instance instance) {
        VkSurfaceKHR surface;
        if (!SDL_Vulkan_CreateSurface(window_, instance, &surface)) {
            IO::ThrowError("SDL Can't create surface");
        }
        return surface; }, width, height);
}

void Application::InitVulkan(std::vector<const char*>& extensions, Vklib::Context::GetSurfaceCallback cb, int windowWidth, int windowHeight) {
    Vklib::Context::Init(extensions, cb);
    Vklib::Context::GetInstance().InitVulkan(windowWidth, windowHeight);

    int maxFlightCount = 2;
    Vklib::DescriptorSetMgr::Init(maxFlightCount);
    renderer_ = std::make_unique<Vklib::Renderer>(maxFlightCount);
    renderer_->SetProjectionMatrix(0, windowWidth, windowHeight, 0, 1, -1);
}

void Application::DestroyVulkan() {
    Vklib::Context::GetInstance().device.waitIdle();
    renderer_.reset();
    Vklib::TextureMgr::GetInstance().Clear();
    Vklib::DescriptorSetMgr::Quit();
    Vklib::Context::Quit();
}

Application::~Application() {
    SDL_Quit();
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

int Application::Run() {
    try {
        float x = 100, y = 100;
        Vklib::Texture* texture1 = Vklib::TextureMgr::GetInstance().Load(GetTestsPath("vkFirstTest/assets/texture.jpg"));
        Vklib::Texture* texture2 = Vklib::TextureMgr::GetInstance().Load(GetTestsPath("vkFirstTest/assets/role.png"));
        while (!shouldClose) {
            while (SDL_PollEvent(&event_)) {
                if (event_.type == SDL_QUIT) {
                    shouldClose = true;
                }
            }
            renderer_->StartRender();
            renderer_->SetDrawColor(Color{1, 0, 0});
            renderer_->DrawRect(Rect{{x, y}, Size{200, 300}}, *texture1);
            renderer_->SetDrawColor(Color{0, 1, 0});
            renderer_->DrawRect(Rect{{500, 100}, Size{200, 300}}, *texture2);
            renderer_->SetDrawColor(Color{0, 0, 1});
            renderer_->DrawLine(Vec2{0, 0}, Vec2{width_, height_});
            renderer_->EndRender();
        }
        DestroyVulkan();
        Vklib::TextureMgr::GetInstance().Destroy(texture1);
        Vklib::TextureMgr::GetInstance().Destroy(texture2);
    } catch (const std::exception& e) {
        IO::PrintLog(LOG_LEVEL_ERROR, "Catch Exception: {}", e.what());
        return -1;
    }
    return 0;
}
