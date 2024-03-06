find_program(GLSLC glslc REQUIRED)

message(STATUS "GLSLC: ${GLSLC}")
message(STATUS "Running glslc to compile shaders ...")
execute_process(COMMAND ${GLSLC} ${PROJECT_SOURCE_DIR}/tests/shaders/vert.glsl -o ${PROJECT_SOURCE_DIR}/tests/shaders/vert.spv)
execute_process(COMMAND ${GLSLC} ${PROJECT_SOURCE_DIR}/tests/shaders/frag.glsl -o ${PROJECT_SOURCE_DIR}/tests/shaders/frag.spv)
message(STATUS "Shaders compiled successfully!")

