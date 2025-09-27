file (GLOB SRC_FILES "*.cpp")

add_library (ui STATIC ${SRC_FILES})

target_link_libraries (ui PRIVATE library)
target_link_libraries (ui PRIVATE imgui)