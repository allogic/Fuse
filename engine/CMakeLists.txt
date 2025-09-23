file (GLOB SRC_FILES "*.c")

add_executable (engine ${SRC_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/sqlite3/src/sqlite3.c")

target_include_directories (engine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/spirv_reflect/include")
target_include_directories (engine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/sqlite3/include")

target_link_libraries (engine PRIVATE library)
target_link_libraries (engine PRIVATE Vulkan::Vulkan)
target_link_libraries (engine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/spirv_reflect/lib/${CMAKE_BUILD_TYPE}/spirv-reflect.lib")
