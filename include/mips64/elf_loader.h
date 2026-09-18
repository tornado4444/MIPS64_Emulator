#pragma once

#include <stdint.h>
#include "mips64/core.h"

typedef enum MIPS64_ELF_MACHINE_MIPS {
	EM_MIPS = 0x08
} MIPS64_ELF_MACHINE_MIPS;

typedef enum Mips64ElfClass {
	MIPS64_ELF_CLASS_NONE = 0,
	MIPS64_ELF_CLASS_32 = 1,
	MIPS64_ELF_CLASS_64 = 2
}Mips64ElfClass;


typedef enum Mips64ElfData {
	MIPS64_ELF_DATA_NONE = 0,
	MIPS64_ELF_DATA_LSB = 1,
	MIPS64_ELF_DATA_MSB = 2
} Mips64ElfData;

MIPS64_API Mips64Status mips64_load_elf(Mips64Emulator* emulator, const void* data, size_t size);