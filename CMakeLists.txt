cmake_minimum_required(VERSION 3.20) 

project(Mips64_Emulator VERSION 0.1.0 DESCRIPTION "Making MIPS64_Emulator" LANGUAGES C)
add_executable(Mips64_Emulator src/main.c)

# FOR ME
# Things you can set on targets
#    target_link_libraries: Other targets; can also pass library names directly
#    target_include_directories: Include directories
#    target_compile_features: The compiler features you need activated, like cxx_std_11
#    target_compile_definitions: Definitions
#    target_compile_options: More general compile flags
#    target_link_directories: Don’t use, give full paths instead (CMake 3.13+)
#    target_link_options: General link flags (CMake 3.13+)
#    target_sources: Add source files
add_library(MIPS64_Library STATIC)

target_sources(MIPS64_Library
	PRIVATE
		src/mips64/core.c
		src/mips64/cpu.c
		src/mips64/memory.c

	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS
			include
		FILES
			include/mips64/core.h
			include/mips64/cpu.h
			include/mips64/memory.h
)
