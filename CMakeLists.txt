cmake_minimum_required(VERSION 3.16.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(vulkan_lib
        LANGUAGES CXX
        DESCRIPTION "A simple vulkan library")

include(cmake/FindVulkan.cmake)
include(cmake/FindGLFW.cmake)
include(cmake/Copydll.cmake)
include(cmake/CompileShaders.cmake)

# 添加宏
add_compile_definitions(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}/")

set(VKLIB_DIR "${PROJECT_SOURCE_DIR}/vklib")
set(VKLIB_SRCS)
file(GLOB VKLIB_SUBDIRS RELATIVE ${VKLIB_DIR} ${VKLIB_DIR}/*)
foreach (SUBDIR ${VKLIB_SUBDIRS})
    if (IS_DIRECTORY ${VKLIB_DIR}/${SUBDIR})
        file(GLOB_RECURSE SUBDIR_SRCS "${VKLIB_DIR}/${SUBDIR}/*.cpp")
        list(APPEND VKLIB_SRCS ${SUBDIR_SRCS})
    endif ()
endforeach ()

add_library(vulkan_lib STATIC ${VKLIB_SRCS})

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/vklib)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/include)

target_link_libraries(vulkan_lib PRIVATE Vulkan::Vulkan)
target_link_libraries(vulkan_lib PRIVATE glfw)
# for debug
target_link_libraries(vulkan_lib PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/lib/fmtd.lib)

file(GLOB TEST_PROJECTS RELATIVE ${PROJECT_SOURCE_DIR}/tests ${PROJECT_SOURCE_DIR}/tests/*)
foreach (TEST_PROJECT ${TEST_PROJECTS})
    if (IS_DIRECTORY ${PROJECT_SOURCE_DIR}/tests/${TEST_PROJECT})
        add_subdirectory(tests/${TEST_PROJECT})
    endif ()
endforeach ()
