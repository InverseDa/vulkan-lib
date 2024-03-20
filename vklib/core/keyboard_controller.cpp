#include "keyboard_controller.hpp"
#include <glm/gtc/constants.hpp>

namespace ida {
void KeyboardMovementController::MoveInPlaneXZ(float dt, ida::IdaGameObject& gameObject) {
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    glm::vec3 rotate{0.0f};
    if (state[keys.lookLeft])
        rotate.y -= 1.f;
    if (state[keys.lookRight])
        rotate.y += 1.f;
    if (state[keys.lookUp])
        rotate.x += 1.f;
    if (state[keys.lookDown])
        rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.rotation = lookSpeed * dt * glm::normalize(rotate);
    }

    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = gameObject.transform.rotation.y;
    const glm::vec3 forwardDir{std::sin(yaw), 0.0f, std::cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x};
    const glm::vec3 upDir{0.0f, -1.0f, 0.0f};

    glm::vec3 moveDir{0.f};
    if (state[keys.moveForward])
        moveDir += forwardDir;
    if (state[keys.moveBackward])
        moveDir -= forwardDir;
    if (state[keys.moveRight])
        moveDir += rightDir;
    if (state[keys.moveLeft])
        moveDir -= rightDir;
    if (state[keys.moveUp])
        moveDir += upDir;
    if (state[keys.moveDown])
        moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.translation += movementSpeed * dt * glm::normalize(moveDir);
    }
}
} // namespace ida