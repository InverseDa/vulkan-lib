#include "app/app.hpp"

int main(int argc, char** argv) {
    return Application::GetInstance("VkAppTest", 800, 600)->Run();
}