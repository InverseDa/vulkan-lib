add_executable(shaderMgrTest)
aux_source_directory(./ SHADER_TEST_SRC)
target_sources(shaderMgrTest PRIVATE ${SHADER_TEST_SRC})
target_link_libraries(shaderMgrTest PUBLIC vulkan_lib Vulkan::Vulkan)
target_include_directories(shaderMgrTest PUBLIC ${PROJECT_SOURCE_DIR}/vklib)
target_include_directories(shaderMgrTest PUBLIC ${PROJECT_SOURCE_DIR}/include)

CopyDLL(shaderMgrTest)
