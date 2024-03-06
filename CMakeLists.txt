cmake_minimum_required(VERSION 3.16.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(vulkan_lib
        LANGUAGES CXX
        DESCRIPTION "A simple vulkan library")

include(cmake/FindVulkan.cmake)
include(cmake/FindSDL2.cmake)
include(cmake/FindGLFW.cmake)
include(cmake/Copydll.cmake)
# 预备阶段 找到glslc程序，用于编译shader
include(cmake/FindGLSLC.cmake)

# 遍历vklib/的每个文件夹里的每个cpp，并加入到编译列表中
# 比如vklib/app/*.cpp
# 比如vklib/vkInstance/*.cpp
set(VKLIB_DIR "${PROJECT_SOURCE_DIR}/vklib")
set(VKLIB_SRCS)
file(GLOB VKLIB_SUBDIRS RELATIVE ${VKLIB_DIR} ${VKLIB_DIR}/*)
foreach (SUBDIR ${VKLIB_SUBDIRS})
    if (IS_DIRECTORY ${VKLIB_DIR}/${SUBDIR})
        file(GLOB_RECURSE SUBDIR_SRCS "${VKLIB_DIR}/${SUBDIR}/*.cpp")
        list(APPEND VKLIB_SRCS ${SUBDIR_SRCS})
    endif ()
endforeach ()

add_executable(vulkan_lib tests/main.cpp ${VKLIB_SRCS})

target_include_directories(vulkan_lib PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(vulkan_lib PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(vulkan_lib PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/include)

target_link_libraries(vulkan_lib PRIVATE Vulkan::Vulkan)
target_link_libraries(vulkan_lib PRIVATE glfw)
target_link_libraries(vulkan_lib PUBLIC SDL2)

CopyDLL(vulkan_lib)