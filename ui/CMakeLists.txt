file (GLOB SRC_FILES "*.cpp")

add_library (ui STATIC ${SRC_FILES}
	"${VENDOR_DIR}/imgui/src/imgui_widgets.cpp"
	"${VENDOR_DIR}/imgui/src/imgui.cpp"
	"${VENDOR_DIR}/imgui/src/imgui_demo.cpp"
	"${VENDOR_DIR}/imgui/src/imgui_draw.cpp"
	"${VENDOR_DIR}/imgui/src/imgui_impl_vulkan.cpp"
	"${VENDOR_DIR}/imgui/src/imgui_impl_win32.cpp"
	"${VENDOR_DIR}/imgui/src/imgui_tables.cpp")

target_include_directories (ui PUBLIC "${VENDOR_DIR}/imgui/include")
target_include_directories (ui PUBLIC "${VENDOR_DIR}/imgui/include/imgui")

target_link_libraries (ui PRIVATE library)
target_link_libraries (ui PRIVATE Vulkan::Vulkan)