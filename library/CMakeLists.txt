file (GLOB SRC_FILES "*.c")

add_library (library STATIC ${SRC_FILES})

add_subdirectory (container)
add_subdirectory (core)
add_subdirectory (database)
add_subdirectory (importer)
add_subdirectory (math)
add_subdirectory (physic3)

target_compile_definitions (library PUBLIC "_CRT_SECURE_NO_WARNINGS")

target_include_directories (library PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_link_libraries (library PUBLIC Vulkan::Vulkan)
target_link_libraries (library PUBLIC spirv_reflect)
target_link_libraries (library PUBLIC sqlite3)
target_link_libraries (library PUBLIC cgltf)
target_link_libraries (library PUBLIC flecs)

target_precompile_headers (library PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lb_pch.h")