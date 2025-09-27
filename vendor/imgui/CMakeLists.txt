add_library (imgui STATIC
	"${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_widgets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/imgui.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_demo.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_impl_vulkan.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_impl_win32.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_tables.cpp")

target_include_directories (imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories (imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/imgui")

target_link_libraries (imgui PUBLIC Vulkan::Vulkan)