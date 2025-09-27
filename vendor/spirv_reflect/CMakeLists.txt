add_library (spirv_reflect STATIC
	"${CMAKE_CURRENT_SOURCE_DIR}/src/spirv_reflect.c")

target_include_directories (spirv_reflect PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories (spirv_reflect PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/spirv_reflect")