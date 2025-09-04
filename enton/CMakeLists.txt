file (GLOB SRC_FILES "*.c")

add_executable (enton ${SRC_FILES})

target_compile_definitions (enton PRIVATE "_CRT_SECURE_NO_WARNINGS")
target_compile_definitions (enton PRIVATE "ROOT_DIR=\"${CMAKE_SOURCE_DIR}\"")

target_include_directories (enton PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories (enton PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/spirv_reflect/include")

target_link_libraries (enton PRIVATE library)
target_link_libraries (enton PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/spirv_reflect/lib/${CMAKE_BUILD_TYPE}/spirv-reflect.lib")

#add_custom_command (TARGET enton POST_BUILD
#  COMMAND "${CMAKE_BINARY_DIR}/enton/enton.exe" "render" "chunk_renderer" "${SHADER_SOURCE_DIR}/chunk/renderer.vert.spv" "${SHADER_SOURCE_DIR}/chunk/renderer.frag.spv"
#  COMMAND "${CMAKE_BINARY_DIR}/enton/enton.exe" "render" "debug_line" "${SHADER_SOURCE_DIR}/debug/line.vert.spv" "${SHADER_SOURCE_DIR}/debug/line.frag.spv"
#)
