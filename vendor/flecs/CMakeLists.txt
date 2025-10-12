add_library (flecs STATIC
	"${CMAKE_CURRENT_SOURCE_DIR}/src/flecs.c")

target_include_directories (flecs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories (flecs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/flecs")