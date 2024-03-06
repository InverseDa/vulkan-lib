#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include <iostream>
#include <vector>

#include "vklib/engine/engine.hpp"

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

    auto render = Vklib::GetRenderer();

    while (!shouldClose) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                shouldClose = true;
            }
        }
        render->Render();
    }

    Vklib::Quit();

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}