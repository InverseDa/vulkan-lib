#include "app/app.h"

int main() {
    Vulkan::Application app(800, 600, false, true, true, "Vulkan Application");
    return app.Run([&](){
        vkCmdBindPipeline(app.GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, app.GetPipeline());
        vkCmdDraw(app.GetCommandBuffer(), 3, 1, 0, 0);
    });
}