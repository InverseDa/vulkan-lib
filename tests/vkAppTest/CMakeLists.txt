add_executable(vkAppTest)
aux_source_directory(./ VK_APP_TEST_SRC)
target_sources(vkAppTest PRIVATE ${VK_APP_TEST_SRC})
target_link_libraries(vkAppTest PUBLIC vulkan_lib SDL2 Vulkan::Vulkan)
target_include_directories(vkAppTest PUBLIC ${PROJECT_SOURCE_DIR}/vklib)
target_include_directories(vkAppTest PUBLIC ${PROJECT_SOURCE_DIR}/include)

CopyDLL(vkAppTest)
