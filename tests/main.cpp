#include "window/window.h"

int main() {
    WindowWrapper window({800, 600}, false, true);
    while (!window.IsClose()) {
        window.ShowTitleFPS();
        window.PollEvents();
        window.SwapBuffers();
    }
}