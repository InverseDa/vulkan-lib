cmake_minimum_required(VERSION 3.16.0)
project(vulkan_lib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(vulkan_lib tests/main.cpp)

find_package(Vulkan REQUIRED)

target_include_directories(vulkan_lib PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(vulkan_lib PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(vulkan_lib PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/include)

target_link_libraries(vulkan_lib PRIVATE Vulkan::Vulkan)
target_link_libraries(vulkan_lib PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/lib/GLFW/libfglfw3.dylib)
