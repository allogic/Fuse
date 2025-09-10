cmake_minimum_required (VERSION 3.10)

project (fuse LANGUAGES C)

set (CMAKE_C_STANDARD 11)
set (CMAKE_C_STANDARD_REQUIRED ON)
set (CMAKE_C_EXTENSIONS OFF)

set (VULKAN_SDK_DIR "C:/VulkanSDK/1.4.321.1")
set (SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shader")

if (NOT CMAKE_BUILD_TYPE)

	set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose a build type (Debug, Release)" FORCE)

endif ()

if (CMAKE_BUILD_TYPE STREQUAL "debug")

	add_compile_definitions (BUILD_DEBUG)

elseif (CMAKE_BUILD_TYPE STREQUAL "release")
	
else ()

	message (FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")

endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")

	add_compile_definitions (OS_WINDOWS)

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")

	add_compile_definitions (OS_LINUX)

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")

	add_compile_definitions (OS_DARWIN)

else ()

	message (FATAL_ERROR "Unknown platform: ${CMAKE_SYSTEM_NAME}")

endif ()

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")

	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od")
	set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /Oi /Ot /GL /fp:fast")

	set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
	set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /OPT:REF /OPT:ICF")

elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")

	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Og")
	set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -ffast-math -ffp-contract=fast -march=native -flto")

	set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
	set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto -Wl,--gc-sections -Wl,--icf=safe")

else ()

	message (FATAL_ERROR "Unknown compiler: ${CMAKE_C_COMPILER_ID}")

endif ()

add_subdirectory (library)
add_subdirectory (engine)
