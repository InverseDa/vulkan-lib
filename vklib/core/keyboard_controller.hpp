#ifndef VULKAN_LIB_KEYBOARD_CONTROLLER_HPP
#define VULKAN_LIB_KEYBOARD_CONTROLLER_HPP

#include "SDL2/SDL.h"

#include "core/game_object.hpp"

namespace ida {
class KeyboardMovementController {
  public:
    struct KeyMappings {
        SDL_Keycode moveForward = SDLK_w;
        SDL_Keycode moveBackward = SDLK_s;
        SDL_Keycode moveLeft = SDLK_a;
        SDL_Keycode moveRight = SDLK_d;
        SDL_Keycode moveUp = SDLK_SPACE;
        SDL_Keycode moveDown = SDLK_LSHIFT;

        SDL_Keycode lookLeft = SDLK_LEFT;
        SDL_Keycode lookRight = SDLK_RIGHT;
        SDL_Keycode lookUp = SDLK_UP;
        SDL_Keycode lookDown = SDLK_DOWN;
    };

    void MoveInPlaneXZ(SDL_Keycode key, float dt, IdaGameObject& gameObject);

    KeyMappings keys{};
    float movementSpeed = 3.0f;
    float lookSpeed = 1.5f;
};
} // namespace ida

#endif // VULKAN_LIB_KEYBOARD_CONTROLLER_HPP
