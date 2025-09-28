file (GLOB SRC_FILES "*.cpp")

add_library (ui STATIC ${SRC_FILES})

target_link_libraries (ui PUBLIC library)
target_link_libraries (ui PUBLIC imgui)

target_precompile_headers (ui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ui_pch.h")