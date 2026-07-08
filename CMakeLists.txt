cmake_minimum_required(VERSION 3.20)

project(MIPS64_Emulator
    VERSION 0.1.2
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(MIPS64_DEFAULT_BUILD_TYPE "Debug" CACHE STRING "Default build type for single-config generators.")
set_property(CACHE MIPS64_DEFAULT_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "${MIPS64_DEFAULT_BUILD_TYPE}" CACHE STRING "Build type." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

if(ANDROID)
    set(MIPS64_BUILD_CLI_DEFAULT OFF)
else()
    set(MIPS64_BUILD_CLI_DEFAULT ON)
endif()

option(MIPS64_BUILD_CLI "Build the command-line emulator runner." ${MIPS64_BUILD_CLI_DEFAULT})
option(MIPS64_WARNINGS_AS_ERRORS "Treat compiler warnings as errors." OFF)

set(MIPS64_CORE_SOURCES
    src/mips64/core.c
    # TODO ADDING THE ANOTHER REALLY NEEDING FOR MIPS64
)

set(MIPS64_CLI_SOURCES
    src/main.c
)

add_library(mips64_project_options INTERFACE)
target_compile_features(mips64_project_options INTERFACE c_std_11)
target_include_directories(mips64_project_options INTERFACE
    ${PROJECT_SOURCE_DIR}/include
)
target_compile_definitions(mips64_project_options INTERFACE
    $<$<PLATFORM_ID:Windows>:MIPS64_PLATFORM_WINDOWS>
    $<$<NOT:$<PLATFORM_ID:Windows>>:MIPS64_PLATFORM_POSIX>
)

add_library(mips64_project_warnings INTERFACE)
if(MSVC)
    target_compile_options(mips64_project_warnings INTERFACE /W4)

    if(MIPS64_WARNINGS_AS_ERRORS)
        target_compile_options(mips64_project_warnings INTERFACE /WX)
    endif()
else()
    target_compile_options(mips64_project_warnings INTERFACE -Wall -Wextra -Wpedantic)

    if(MIPS64_WARNINGS_AS_ERRORS)
        target_compile_options(mips64_project_warnings INTERFACE -Werror)
    endif()
endif()

add_library(mips64_core STATIC
    ${MIPS64_CORE_SOURCES}
)
target_link_libraries(mips64_core
    PUBLIC
        mips64_project_options
    PRIVATE
        mips64_project_warnings
)

if(MIPS64_BUILD_CLI)
    add_executable(mips64_emu
        ${MIPS64_CLI_SOURCES}
    )
    target_link_libraries(mips64_emu
        PRIVATE
            mips64_core
            mips64_project_warnings
    )

    set_target_properties(mips64_emu PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()
