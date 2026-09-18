#pragma once

#include <stdint.h>

#include "mips64/core.h"

#define EI_NIDENT 16

/*
	64-Bit Data Types
	----------------------------------------------------------------------
	|   NAME        |  Size  |   Alignment   |           PURPOSE         |
	|	Elf64_Addr  |   4    |      4        |  Unsigned program address |
	|	Elf64_Half	|	2	 |		2		 |  Unsigned medium integer  |
	|	Elf64_Off	|	4	 |		4		 |	Unsigned file offset	 |
	|	Elf64_Sword	|	4	 |		4		 |	Signed large integer	 |
	|	Elf64_Word	|	4	 |		4		 |	Unsigned large integer	 |
	-----------------------------------------------------------------------
*/

typedef uint32_t  Elf64_Addr;
typedef uint16_t  Elf64_Half;
typedef uint32_t  Elf64_Off;
typedef int32_t   Elf64_Sword;
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


typedef union {
	unsigned char e_ident[EI_NIDENT];

	Elf64_Half    e_type;
	Elf64_Half    e_machine;
	Elf64_Word    e_version;
	Elf64_Addr    e_entry;
	Elf64_Off     e_phoff;
	Elf64_Off	  e_shoff;
	Elf64_Word	  e_flags;
	Elf64_Half    e_ehsize;
	Elf64_Half    e_phentsize;
	Elf64_Half    e_phnum;
	Elf64_Half    e_shentsize;
	Elf64_Half    e_shnum;
	Elf64_Half    e_shstrndx;

	struct {
		uint32_t magic;
		uint8_t  elf_class;
		uint8_t  data_encoding;
		uint8_t  version;
		uint8_t  os_abi;
		uint8_t  abi_version;
		uint8_t padding[7];
	} fields;
} Elf64_Ehdr;

typedef enum Mips64ElfClass {
	MIPS64_ELF_CLASS_NONE = 0,
	MIPS64_ELF_CLASS_32 = 1,
	MIPS64_ELF_CLASS_64 = 2
} Mips64ElfClass;

typedef enum Mips64ElfData {
	MIPS64_ELF_DATA_NONE = 0,
	MIPS64_ELF_DATA_LSB = 1,
	MIPS64_ELF_DATA_MSB = 2
} Mips64ElfData;

typedef enum Mips64_ELF_TYPE_MIPS {
	ET_NONE = 0, ET_REL = 1, ET_EXEC = 2,
	ET_DYN = 3,  ET_CORE = 4, ET_LOPROC = 0x00FF,
	ET_HIPROC = 0xFFFF
} Mips64ElfE_Type;

typedef enum MIPS64_ELF_MACHINE_MIPS {
	EM_MIPS = 0x08
} MIPS64_ELF_MACHINE_MIPS;

typedef enum MIPS64_ELF_VERSION_MIPS {
	EV_NONE = 0,
	EV_CURRENT = 1
} MIPS64_ELF_VERSION_MIPS;

MIPS64_API Mips64Status mips64_load_elf(Mips64Emulator* emulator, const void* data, size_t size);