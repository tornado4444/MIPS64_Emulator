cmake_minimum_required(VERSION 3.20) 

project(Mips64_Emulator VERSION 0.1.0 DESCRIPTION "Making MIPS64_Emulator" LANGUAGES C)
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
add_library(MIPS64_Core SHARED)

set(HEADER_LIST
	include/mips64/core.h
	include/mips64/cpu.h
	include/mips64/memory.h
	include/mips64/decode.h
	include/mips64/execute.h
)

# ONLY FOR MSVC
# Debug -> /MDd
# other -> /MD
if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY 
	"MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

target_sources(MIPS64_Core
	PRIVATE
		src/mips64/core.c
		src/mips64/cpu.c
		src/mips64/memory.c
		src/mips64/decode.c
		src/mips64/execute.c

	PUBLIC
		${HEADER_LIST}
)

target_include_directories(MIPS64_Core
	PUBLIC
		include
)

target_compile_features(MIPS64_Core
	PUBLIC
		c_std_11
)

# Compiler warnings

if(MSVC)
	target_compile_options(MIPS64_Core
		PRIVATE
			/W4
	)
else() 
	target_compile_options(MIPS64_Core
		PRIVATE
			-Wall
			-Wextra
			-Wpedantic
			-Werror
	)
endif()

# CLI executable

add_executable(Mips64_Emulator src/main.c)

target_link_libraries(Mips64_Emulator
    PRIVATE
        MIPS64_Core
)