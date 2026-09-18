#pragma once

#include <stdint.h>

#include "mips64/core.h"

#define EI_NIDENT 16

typedef uint32_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Off;
typedef uint32_t  Elf64_Sword;
typedef uint32_t  Elf64_Word;

/*
	The architecture of the hardware platform for which the file is created. 
	The most important thing to understand is that there is a difference between the instructions in the ELF header file.
	For MIPS64 instruction been(but also it's can be for other Mips):
	---------------------------------------
	|   NAME    |  VALUE  |   Description |  
	|	EM_MIPS |   0x08  |   MIPS        |
	---------------------------------------
	But not to be confused with MIPS R3000 Little-endian:
	-------------------------------------------------------
	|   NAME    |  VALUE  |	      Description              |
	|	EM_MIPS |   0x0À  |   R3000 Little-endian          |
	--------------------------------------------------------
*/

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