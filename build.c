/*
	* build.c - Build system for MIPS64_Emulator
	*
	* This build system is heavily inspired by and based on the build architecture
	* from the 'gimsatul' project by Armin Biere.
	*
	* Original build system code: Copyright (c) 2020-2024 Armin Biere, JKU Linz
	* Original Source: https://github.com/arminbiere/gimsatul/blob/master/build.c
	* Licensed under the MIT License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32 // USING FOR THE OS WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#define	_WINUSER_
	#define	_WINGDI_
	#define	_IMM_
	#define	_WINCON_
	#include <windows.h>
	#include <direct.h>
	#include <io.h>
	#include <shellapi> 
	#define MKDIR(path) _mkdir(path) 
	#define EXE_EXT ".exe"
#else
	#include <sys/stat.h>
	#include <sys/types.h>
	#define MKDIR(path) mkdir(path, 0755)
	#define EXE_EXT ""
#endif

#ifndef BUILD_WARN_DEPRECATED
ifndef BUILD_WARN_DEPRECATED
#        if defined(__GNUC__) || defined(__clang__)
#            define BUILD_WARN_DEPRECATED(message) __attribute__((deprecated(message)))
#        elif defined(_MSC_VER)
#            define BUILD_WARN_DEPRECATED(message) __declspec(deprecated(message))
#        else
#            define BUILD_WARN_DEPRECATED(...)
#        endif
#    endif
#else
#	 define BUILD_WARN_DEPRECATED(...)
#endif

#ifdef _WIN32
	#define BUILD_LINE_END "\r\n"
#else
	#define BUILD_LINE_END "\n"
#endif

#if defined(__GNUC__) || defined(__clang__) 
	/* TODO Adding the alloc_size for the compiler that the function return value points GCC compiler for value points to memory
	void* my_calloc(size_t, size_t) __attribute__((alloc_size(1,2)))
    void my_realloc(void*, size_t) __attribute__((alloc_size(2)))
	*/
#	ifndef __MINGW_PRINTF_FORMAT
#		define BUILD_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#   else
#		define BUILD_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (gnu_printf, STRING_INDEX, FIRST_TO_CHECK)))
#	endif
#else
	/* MSVC don't support syntax __attribute__ ((format (printf, STRING_INDEX, FIRST_TO_CHECK)))
	   this format is only for UNIX been working 	
	*/
#   define BUILD_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

#define MAX_FILES 64
#define MAX_PATH_LEN 52
#define CMD_LEN 16384

// WROTE FOR THE DIFFERENT COMPILERS FOR DIFFERENT OS
typedef enum Compiler {
	COMPILER_MSVC,
	COMPILER_GCC,
	COMPILER_CLANG
} Compiler;

// FOR THE DEBUG/RELEASE MODE
typedef enum BuildMode {
	BUILD_DEBUG,
	BUILD_RELEASE
} BuildMode;

// RELEASE THE LOGGER
typedef enum Logger {
	INFO,
	WARNING,
	ERROR,
	NO_LOGS
} Logger;

typedef struct BuildConfig {
	Compiler Compiler;
	BuildMode buildMode;

	const char* compiler_name;
	const char* mode_name;

	char obj_dir[MAX_PATH_LEN];
	char output_file[MAX_PATH_LEN];
} BuildConfig;

static int str_eq(const char* a, const char* b) {
	return strcmp(a, b) == 0;
}

static void run_cmd(const char* cmd) {
	printf("\nCMD %s\n", cmd);

	int result = system(cmd);

	if (result != 0) {
		printf("\n[ERROR] Command failed with code: %d\n", result);
		exit(1);
	}
}

static void make_dir(const char* path) {
	MKDIR(path);
}




