file (GLOB SRC_FILES "*.c")

add_executable (engine ${SRC_FILES})

add_subdirectory (component)
add_subdirectory (scene)
add_subdirectory (system)

target_link_libraries (engine PUBLIC library)
target_link_libraries (engine PUBLIC ui)

target_precompile_headers (engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/eg_pch.h")