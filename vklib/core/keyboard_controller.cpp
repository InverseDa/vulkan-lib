#include "keyboard_controller.hpp"
#include <glm/gtc/constants.hpp>

namespace ida {
void KeyboardMovementController::MoveInPlaneXZ(SDL_Keycode key, float dt, ida::IdaGameObject& gameObject) {
    glm::vec3 rotate{0.0f};
    if (key == keys.lookLeft)
        rotate.y -= 1.f;
    if (key == keys.lookRight)
        rotate.y += 1.f;
    if (key == keys.lookUp)
        rotate.x += 1.f;
    if (key == keys.lookDown)
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
    if (key == keys.moveForward)
        moveDir += forwardDir;
    if (key == keys.moveBackward)
        moveDir -= forwardDir;
    if (key == keys.moveRight)
        moveDir += rightDir;
    if (key == keys.moveLeft)
        moveDir -= rightDir;
    if (key == keys.moveUp)
        moveDir += upDir;
    if (key == keys.moveDown)
        moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.translation += movementSpeed * dt * glm::normalize(moveDir);
    }
}
} // namespace ida