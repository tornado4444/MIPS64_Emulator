/*
    * build.c - Build system for MIPS64_Emulator
    *
    * This build system is inspired by the "no build system" style:
    * compile this file once, then use the produced executable to build
    * the real project.
    *
    * References used while designing this file:
    * - nob.h by Tsoding: https://github.com/tsoding/nob.h
    * - GCC function attributes:
    *   https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Function-Attributes.html
    *
    * The implementation below is intentionally small. It does not try to copy
    * nob.h; it only keeps the useful idea that build steps can be expressed
    * directly in C.
*/

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32 // USING FOR THE OS WINDOWS
    #include <direct.h>
    #include <io.h>
    #define MKDIR(path) _mkdir(path)
    #define ACCESS(path) _access(path, 0)
    #define EXE_EXT ".exe"
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
    #define MKDIR(path) mkdir(path, 0755)
    #define ACCESS(path) access(path, F_OK)
    #define EXE_EXT ""
#endif

#ifdef BUILD_WARN_DEPRECATED
    #if defined(__GNUC__) || defined(__clang__)
        #define BUILD_DEPRECATED(message) __attribute__((deprecated(message)))
    #elif defined(_MSC_VER)
        #define BUILD_DEPRECATED(message) __declspec(deprecated(message))
    #else
        #define BUILD_DEPRECATED(message)
    #endif
#else
    #define BUILD_DEPRECATED(message)
#endif

#if defined(__GNUC__) || defined(__clang__)
    /*
        GCC/Clang can check printf-like functions at compile time.
        On MinGW the printf format can be different, so we use its own macro
        when it is available.
    */
    #ifdef __MINGW_PRINTF_FORMAT
        #define BUILD_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) \
            __attribute__((format(__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
    #else
        #define BUILD_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) \
            __attribute__((format(printf, STRING_INDEX, FIRST_TO_CHECK)))
    #endif
#else
    #define BUILD_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

#define BUILD_UNUSED(value) (void)(value)
#define BUILD_ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))

#define MAX_FILES 64
#define MAX_PATH_LEN 512
#define CMD_LEN 16384

// WROTE FOR THE DIFFERENT COMPILERS FOR DIFFERENT OS
typedef enum Compiler {
    COMPILER_GCC,
    COMPILER_CLANG,
    COMPILER_MSVC
} Compiler;

// FOR THE DEBUG/RELEASE MODE
typedef enum BuildMode {
    BUILD_DEBUG,
    BUILD_RELEASE
} BuildMode;

// RELEASE THE LOGGER
typedef enum BuildLogLevel {
    BUILD_LOG_INFO,
    BUILD_LOG_WARNING,
    BUILD_LOG_ERROR,
    BUILD_LOG_NONE
} BuildLogLevel;

typedef struct BuildConfig {
    Compiler compiler;
    BuildMode mode;

    const char* compiler_name;
    const char* mode_name;

    char obj_dir[MAX_PATH_LEN];
    char output_file[MAX_PATH_LEN];
} BuildConfig;

static BuildLogLevel minimal_log_level = BUILD_LOG_INFO;

static int str_eq(const char* a, const char* b) {
    return strcmp(a, b) == 0;
}

static const char* log_level_name(BuildLogLevel level) {
    switch (level) {
        case BUILD_LOG_INFO:    return "INFO";
        case BUILD_LOG_WARNING: return "WARNING";
        case BUILD_LOG_ERROR:   return "ERROR";
        case BUILD_LOG_NONE:    return "NONE";
        default:                return "UNKNOWN";
    }
}

static void build_log(BuildLogLevel level, const char* fmt, ...)
    BUILD_PRINTF_FORMAT(2, 3);

static void build_log(BuildLogLevel level, const char* fmt, ...) {
    if (level < minimal_log_level || level == BUILD_LOG_NONE) {
        return;
    }

    FILE* stream = (level == BUILD_LOG_ERROR) ? stderr : stdout;

    fprintf(stream, "[%s] ", log_level_name(level));

    va_list args;
    va_start(args, fmt);
    vfprintf(stream, fmt, args);
    va_end(args);

    fputc('\n', stream);
}

static void append_cmd(char* cmd, size_t cmd_size, const char* fmt, ...)
    BUILD_PRINTF_FORMAT(3, 4);

static void append_cmd(char* cmd, size_t cmd_size, const char* fmt, ...) {
    size_t used = strlen(cmd);

    if (used >= cmd_size) {
        build_log(BUILD_LOG_ERROR, "Command buffer is already full.");
        exit(1);
    }

    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(cmd + used, cmd_size - used, fmt, args);
    va_end(args);

    if (written < 0 || (size_t)written >= cmd_size - used) {
        build_log(BUILD_LOG_ERROR, "Command is too long.");
        exit(1);
    }
}

static void run_cmd(const char* cmd) {
    build_log(BUILD_LOG_INFO, "CMD %s", cmd);

    int result = system(cmd);

    if (result != 0) {
        build_log(BUILD_LOG_ERROR, "Command failed with code: %d", result);
        exit(1);
    }
}

static int path_exists(const char* path) {
    return ACCESS(path) == 0;
}

static void make_dir(const char* path) {
    if (path_exists(path)) {
        return;
    }

    if (MKDIR(path) != 0 && errno != EEXIST) {
        build_log(BUILD_LOG_ERROR, "Could not create directory '%s': %s", path, strerror(errno));
        exit(1);
    }
}

static void clean_project(void) {
#ifdef _WIN32
    run_cmd("if exist obj rmdir /S /Q obj");
    run_cmd("if exist bin rmdir /S /Q bin");
#else
    run_cmd("rm -rf obj bin");
#endif

    build_log(BUILD_LOG_INFO, "Clean done.");
}

static Compiler default_compiler(void) {
#if defined(_MSC_VER) && !defined(__clang__)
    return COMPILER_MSVC;
#elif defined(__clang__)
    return COMPILER_CLANG;
#else
    return COMPILER_GCC;
#endif
}

static Compiler parse_compiler(const char* arg) {
    if (str_eq(arg, "gcc")) {
        return COMPILER_GCC;
    }

    if (str_eq(arg, "clang")) {
        return COMPILER_CLANG;
    }

    if (str_eq(arg, "msvc") || str_eq(arg, "cl")) {
        return COMPILER_MSVC;
    }

    build_log(BUILD_LOG_ERROR, "Unknown compiler: %s", arg);
    build_log(BUILD_LOG_ERROR, "Available compilers: gcc, clang, msvc");
    exit(1);
}

static int is_compiler_arg(const char* arg) {
    return str_eq(arg, "gcc") ||
           str_eq(arg, "clang") ||
           str_eq(arg, "msvc") ||
           str_eq(arg, "cl");
}

static BuildMode parse_mode(const char* arg) {
    if (str_eq(arg, "debug")) {
        return BUILD_DEBUG;
    }

    if (str_eq(arg, "release")) {
        return BUILD_RELEASE;
    }

    build_log(BUILD_LOG_ERROR, "Unknown build mode: %s", arg);
    build_log(BUILD_LOG_ERROR, "Available modes: debug, release");
    exit(1);
}

static int is_mode_arg(const char* arg) {
    return str_eq(arg, "debug") || str_eq(arg, "release");
}

static const char* get_compiler_name(Compiler compiler) {
    switch (compiler) {
        case COMPILER_GCC:   return "gcc";
        case COMPILER_CLANG: return "clang";
        case COMPILER_MSVC:  return "msvc";
        default:             return "unknown";
    }
}

static const char* get_mode_name(BuildMode mode) {
    switch (mode) {
        case BUILD_DEBUG:   return "debug";
        case BUILD_RELEASE: return "release";
        default:            return "unknown";
    }
}

static void make_safe_obj_name(const char* src, char* out, size_t out_size) {
    size_t j = 0;

    for (size_t i = 0; src[i] != '\0' && j + 1 < out_size; i++) {
        unsigned char c = (unsigned char)src[i];

        if (isalnum(c)) {
            out[j++] = (char)c;
        } else {
            out[j++] = '_';
        }
    }

    out[j] = '\0';
}

static void make_obj_path(
    const BuildConfig* config,
    const char* src,
    char* out,
    size_t out_size
) {
    char safe_name[MAX_PATH_LEN];
    make_safe_obj_name(src, safe_name, sizeof(safe_name));

    const char* obj_ext = ".o";

    if (config->compiler == COMPILER_MSVC) {
        obj_ext = ".obj";
    }

    snprintf(out, out_size, "%s/%s%s", config->obj_dir, safe_name, obj_ext);
}

static void compile_c_file(
    const BuildConfig* config,
    const char* src,
    char* out_obj,
    size_t out_obj_size
) {
    if (!path_exists(src)) {
        build_log(BUILD_LOG_ERROR, "Source file does not exist: %s", src);
        exit(1);
    }

    make_obj_path(config, src, out_obj, out_obj_size);

    char cmd[CMD_LEN];
    cmd[0] = '\0';

#ifdef _WIN32
    const char* platform_define = "MIPS64_PLATFORM_WINDOWS";
#else
    const char* platform_define = "MIPS64_PLATFORM_POSIX";
#endif

    if (config->compiler == COMPILER_MSVC) {
        const char* mode_flags = "/Zi /Od /DDEBUG";

        if (config->mode == BUILD_RELEASE) {
            mode_flags = "/O2 /DNDEBUG";
        }

        append_cmd(
            cmd,
            sizeof(cmd),
            "cl /nologo /TC /std:c11 /W4 "
            "/Iinclude "
            "/D_CRT_SECURE_NO_WARNINGS "
            "/D%s "
            "%s "
            "/c \"%s\" "
            "/Fo\"%s\"",
            platform_define,
            mode_flags,
            src,
            out_obj
        );
    } else {
        const char* cc = get_compiler_name(config->compiler);
        const char* mode_flags = "-g -O0 -DDEBUG";

        if (config->mode == BUILD_RELEASE) {
            mode_flags = "-O2 -DNDEBUG";
        }

        append_cmd(
            cmd,
            sizeof(cmd),
            "%s "
            "-std=c11 "
            "-Wall -Wextra -pedantic "
            "-Iinclude "
            "-D%s "
            "%s "
            "-c \"%s\" "
            "-o \"%s\"",
            cc,
            platform_define,
            mode_flags,
            src,
            out_obj
        );
    }

    run_cmd(cmd);
}

static void link_executable(
    const BuildConfig* config,
    const char objects[MAX_FILES][MAX_PATH_LEN],
    size_t object_count
) {
    char cmd[CMD_LEN];
    cmd[0] = '\0';

    if (config->compiler == COMPILER_MSVC) {
        append_cmd(cmd, sizeof(cmd), "cl /nologo /Fe\"%s\"", config->output_file);
    } else {
        append_cmd(
            cmd,
            sizeof(cmd),
            "%s -o \"%s\"",
            get_compiler_name(config->compiler),
            config->output_file
        );
    }

    for (size_t i = 0; i < object_count; i++) {
        append_cmd(cmd, sizeof(cmd), " \"%s\"", objects[i]);
    }

    run_cmd(cmd);
}

static void init_config(BuildConfig* config, Compiler compiler, BuildMode mode) {
    memset(config, 0, sizeof(*config));

    config->compiler = compiler;
    config->mode = mode;
    config->compiler_name = get_compiler_name(config->compiler);
    config->mode_name = get_mode_name(config->mode);

    snprintf(
        config->obj_dir,
        sizeof(config->obj_dir),
        "obj/%s_%s",
        config->compiler_name,
        config->mode_name
    );

    snprintf(
        config->output_file,
        sizeof(config->output_file),
        "bin/mips64_emu%s",
        EXE_EXT
    );
}

static void build_project(const BuildConfig* config) {
    static const char* sources[] = {
        /*
            This is the minimal entry point for the project right now.
            Add future emulator modules here when they exist, for example:

                "src/core/cpu.c",
                "src/core/memory.c",
        */
        "src/mips64/core.c",
        "src/main.c",
    };

    char objects[MAX_FILES][MAX_PATH_LEN];
    size_t object_count = 0;

    if (BUILD_ARRAY_LEN(sources) > MAX_FILES) {
        build_log(BUILD_LOG_ERROR, "Too many source files. Increase MAX_FILES.");
        exit(1);
    }

    make_dir("obj");
    make_dir("bin");
    make_dir(config->obj_dir);

    build_log(BUILD_LOG_INFO, "Compiler: %s", config->compiler_name);
    build_log(BUILD_LOG_INFO, "Mode:     %s", config->mode_name);
    build_log(BUILD_LOG_INFO, "Obj dir:  %s", config->obj_dir);
    build_log(BUILD_LOG_INFO, "Output:   %s", config->output_file);

    for (size_t i = 0; i < BUILD_ARRAY_LEN(sources); i++) {
        compile_c_file(
            config,
            sources[i],
            objects[object_count],
            sizeof(objects[object_count])
        );

        object_count++;
    }

    link_executable(config, objects, object_count);

    build_log(BUILD_LOG_INFO, "Build successful: %s", config->output_file);
}

static void usage(const char* program) {
    printf("Usage:\n");
    printf("  %s [gcc|clang|msvc] [debug|release]\n", program);
    printf("  %s clean\n", program);
    printf("  %s help\n", program);
    printf("\n");
    printf("Examples:\n");
    printf("  %s\n", program);
    printf("  %s clang debug\n", program);
    printf("  %s release\n", program);
    printf("  %s clean\n", program);
}

int main(int argc, char** argv) {
    Compiler compiler = default_compiler();
    BuildMode mode = BUILD_DEBUG;

    if (argc >= 2 && (str_eq(argv[1], "help") || str_eq(argv[1], "--help") || str_eq(argv[1], "-h"))) {
        usage(argv[0]);
        return 0;
    }

    if (argc >= 2 && str_eq(argv[1], "clean")) {
        clean_project();
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        const char* arg = argv[i];

        if (is_compiler_arg(arg)) {
            compiler = parse_compiler(arg);
        } else if (is_mode_arg(arg)) {
            mode = parse_mode(arg);
        } else {
            build_log(BUILD_LOG_ERROR, "Unknown argument: %s", arg);
            usage(argv[0]);
            return 1;
        }
    }

    BuildConfig config;
    init_config(&config, compiler, mode);
    build_project(&config);

    return 0;
}
