file (GLOB SRC_FILES "*.c")

add_library (library STATIC ${SRC_FILES})

add_subdirectory (core)
add_subdirectory (database)
add_subdirectory (math)

target_compile_definitions (library PUBLIC "_CRT_SECURE_NO_WARNINGS")

target_include_directories (library PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories (library PUBLIC "${VENDOR_DIR}/spirv_reflect/include")
target_include_directories (library PUBLIC "${VENDOR_DIR}/sqlite3/include")

target_link_libraries (library PUBLIC Vulkan::Vulkan)
target_link_libraries (library PUBLIC "${VENDOR_DIR}/spirv_reflect/lib/${CMAKE_BUILD_TYPE}/spirv-reflect-static.lib")