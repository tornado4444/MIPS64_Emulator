#include <stdio.h>

#include "mips64/core.h"

int main(int argc, char** argv) {
    printf("%s bootstrap\n", mips64_core_name());
    printf("Core version major: %d\n", mips64_core_version_major());
    printf("Arguments passed to emulator: %d\n", argc - 1);

    for (int i = 1; i < argc; i++) {
        printf("  argv[%d] = %s\n", i, argv[i]);
    }

    return 0;
}
