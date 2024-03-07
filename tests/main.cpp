#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include <iostream>
#include <vector>

#include "vklib/engine/engine.hpp"
#include "vklib/math/vec2.hpp"
#include "vklib/math/uniform.hpp"

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Vulkan Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
    if (!window) {
        SDL_Log("Create window failed, ERROR MSG: %s", SDL_GetError());
        exit(2);
    }
    bool shouldClose = false;
    SDL_Event event;

    unsigned int count;
    SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
    std::vector<const char*> extensions(count);
    SDL_Vulkan_GetInstanceExtensions(window, &count, extensions.data());

    Vklib::Init(extensions, [&](vk::Instance instance) {
        VkSurfaceKHR surface;
        if (!SDL_Vulkan_CreateSurface(window, instance, &surface)) {
            IO::ThrowError("SDL Can't create surface");
        }
        return surface; }, 1024, 720);

    auto renderer = Vklib::GetRenderer();

    float x = 100, y = 100;
    renderer->SetDrawColor(Color{0, 1, 0});
    while (!shouldClose) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                shouldClose = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_a) {
                    x -= 10;
                }
                if (event.key.keysym.sym == SDLK_d) {
                    x += 10;
                }
                if (event.key.keysym.sym == SDLK_w) {
                    y -= 10;
                }
                if (event.key.keysym.sym == SDLK_s) {
                    y += 10;
                }
                if (event.key.keysym.sym == SDLK_0) {
                    renderer->SetDrawColor(Color{1, 0, 0});
                }
                if (event.key.keysym.sym == SDLK_1) {
                    renderer->SetDrawColor(Color{0, 1, 0});
                }
                if (event.key.keysym.sym == SDLK_2) {
                    renderer->SetDrawColor(Color{0, 0, 1});
                }
            }
        }
        renderer->Render(Rect{
                Vec2{x, y},
                Size{200, 300}
        });
    }

    Vklib::Quit();

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}