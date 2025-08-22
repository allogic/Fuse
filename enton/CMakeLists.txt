file (GLOB SRC_FILES "*.c")

add_executable (enton ${SRC_FILES})

target_compile_definitions (enton PRIVATE "_CRT_SECURE_NO_WARNINGS")
target_compile_definitions (enton PRIVATE "ROOT_DIR=\"${CMAKE_SOURCE_DIR}\"")

target_include_directories (enton PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_link_libraries (enton PRIVATE library)
