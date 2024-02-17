#include "window/window.h"
#include "synchronize/fence.h"
#include "synchronize/semaphore.h"

int main() {
    WindowWrapper window({800, 600}, false, true);

    Vulkan::Fence fence(VK_FENCE_CREATE_SIGNALED_BIT);
    Vulkan::Semaphore imageAvailableSem;
    Vulkan::Semaphore renderingIsFinishedSem;

    while (!window.IsClose()) {
        window.ShowTitleFPS();
        window.PollEvents();
        window.SwapBuffers();
    }
}