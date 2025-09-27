file (GLOB SRC_FILES "*.c")

add_executable (engine ${SRC_FILES})

target_link_libraries (engine PRIVATE library)
target_link_libraries (engine PRIVATE ui)

target_precompile_headers (engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/eg_pch.h")