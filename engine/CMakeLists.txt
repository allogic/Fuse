file (GLOB SRC_FILES "*.c")

add_executable (engine ${SRC_FILES} "${VENDOR_DIR}/sqlite3/src/sqlite3.c")

target_include_directories (engine PRIVATE "${VENDOR_DIR}/spirv_reflect/include")
target_include_directories (engine PRIVATE "${VENDOR_DIR}/sqlite3/include")

target_link_libraries (engine PRIVATE library)
target_link_libraries (engine PRIVATE ui)
target_link_libraries (engine PRIVATE Vulkan::Vulkan)
target_link_libraries (engine PRIVATE "${VENDOR_DIR}/spirv_reflect/lib/${CMAKE_BUILD_TYPE}/spirv-reflect-static.lib")
