#include "engine.hpp"
#include "core/context.hpp"

namespace Engine {
void Init() {
    Vklib::Context::Init();
}

void Quit() {
    Vklib::Context::Quit();
}

} // namespace Engine