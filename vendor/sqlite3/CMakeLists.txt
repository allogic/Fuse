add_library (sqlite3 STATIC
	"${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite3.c")

target_include_directories (sqlite3 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")