file (GLOB SRC_FILES "*.c")

add_executable (engine ${SRC_FILES})

add_subdirectory (physics)
add_subdirectory (renderer)

target_compile_definitions (engine PRIVATE "_CRT_SECURE_NO_WARNINGS")
target_compile_definitions (engine PRIVATE "ROOT_DIR=\"${CMAKE_SOURCE_DIR}\"")

target_include_directories (engine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories (engine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/spirv_reflect/include")

target_link_libraries (engine PRIVATE library)
target_link_libraries (engine PRIVATE Vulkan::Vulkan)
target_link_libraries (engine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/spirv_reflect/lib/${CMAKE_BUILD_TYPE}/spirv-reflect.lib")
