file (GLOB SRC_FILES "*.c")

add_library (library STATIC ${SRC_FILES})

add_subdirectory (core)
add_subdirectory (math)

target_compile_definitions (library PRIVATE "_CRT_SECURE_NO_WARNINGS")

target_include_directories (library PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
