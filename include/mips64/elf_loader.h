#pragma once

#include <stdint.h>

#include "mips64/core.h"

#define EI_NIDENT 16

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'


#define ELF64_ST_BIND(i)	((i) >> 4)
#define ELF64_ST_BIND(i)	((i) &0XF)
#define ELF64_ST_INFO(b, t) (((b << 4)) + ((t) & 0XF))


#define ELF64_R_SYM(i)      ((i) >> 8)
#define ELF64_R_TYPE(i)     ((unsigned char)(i)
#define ELF64_R_SYM(i)      (((s)<<8)+(unsigned char)(t))
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

typedef uint32_t     Elf64_Addr;
typedef uint16_t     Elf64_Half;
typedef uint32_t     Elf64_Off;
typedef signed int   Elf64_Sword;
typedef uint32_t     Elf64_Word;

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

typedef enum MIPS64_ELF_Ident_Indexes {
	EI_MAG0 = 0, EI_MAG1 = 1, EI_MAG2 = 2,
	EI_MAG3 = 3, EI_CLASS = 4, EI_DATA = 5,
	EI_VERSION = 6, EI_VERSION = 7 // EI_NIDENT = 16 we declared it as a macro #define
} MIPS64_ELF_Ident_Indexes;

typedef enum Mips64ElfClass {
	MIPS64_ELF_CLASS_NONE = 0,
	MIPS64_ELF_CLASS_32   = 1,
	MIPS64_ELF_CLASS_64   = 2
} Mips64ElfClass;

typedef enum Mips64ElfData {
	MIPS64_ELF_DATA_NONE  = 0,
	MIPS64_ELF_DATA_LSB   = 1,
	MIPS64_ELF_DATA_MSB   = 2
} Mips64ElfData;

typedef enum Mips64ElfSSI {
	SHN_UNDEF = 0,
	SHN_LORESERVE = 0xFF00,
	SHN_LOPROC = 0xFF00,
	SHN_HIPROC = 0xFF1F,
	SHN_ABS = 0xFFF1,
	SHN_COMMON = 0xFFF2,
	SHN_HIRESERVE = 0xFFFF
} Mips64ElfSSI;


typedef union Efl64_Shdr {
	Elf64_Word sh_name;
	Elf64_Word sh_type;

	enum {
		SHT_NULL	 = 0,
		SHT_PROGBITS = 1,
		SHT_SYMTAB   = 2,
		SHT_STRTAB   = 3,
		SHT_RELA     = 4,
		SHT_HASH     = 5,
		SHT_DYNAMIC  = 6,
		SHT_NOTE     = 7,
		SHT_NOBITS   = 8,
		SHT_REL      = 9,
		SHT_SHLIB    = 10,
		SHT_DYNSYM   = 11,
		SHT_LOPROC   = 0x70000000,
		SHT_HIPROC   = 0x7FFFFFFF,
		SHT_LOUSER   = 0x80000000,
		SHT_HIUSER   = 0xFFFFFFFF
	} elf_sh_type;

	Elf64_Word sh_flags;

	enum {
		SHF_WRITE     = 0x1,
		SHF_ALLOC     = 0x2,
		SHF_EXECINSTR = 0x4,
		SHF_MASKPROC  = 0xF0000000
	} elf_sh_flags;

	Elf64_Addr sh_addr;
	Elf64_Off  sh_offset;
	Elf64_Word sh_size;
	Elf64_Word sh_link;
	Elf64_Word sh_info;
	Elf64_Word sh_addralign;
	Elf64_Word sh_entsize
} Efl64_Shdr;

typedef struct Elf64_Sym {
	Elf64_Word    st_name;
	Elf64_Addr    st_value;
	Elf64_Word    st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf64_Half    st_shndx;
} Elf64_Sym;

typedef enum Elf64_ST_BIND {
	STB_LOCAL  = 0,
	STB_GLOBAL = 1,
	STB_WEAK   = 2,
	STB_LOPROC = 13,
	STB_HIPROC = 15
} Elf64_ST_BIND;

typedef enum Elf64_ST_TYPE {
	STT_NOTYPE  = 0,
	STT_OBJECT  = 1,
	STT_FUNC    = 2,
	STT_SECTION = 3,
	STT_FILE    = 4,
	STT_LOPROC  = 13,
	STT_HIPROC  = 15
} Elf64_ST_TYPE;

typedef struct Elf64_Rel {
	Elf64_Addr r_offset;
	Elf64_Word r_info;
} Elf64_Rel;

typedef struct Elf64_Rela {
	Elf64_Addr  r_offset;
	Elf64_Word  r_info;
	Elf64_Sword r_addend;
} Elf64_Rela;

// ------------------------------PROGRAM HEADER------------------------
typedef union Elf64_Phdr {
	Elf64_Word p_type;

	enum elf_p_type {
		PT_NULL = 0,
		PT_LOAD = 1,
		PT_DYNAMIC = 2,
		PT_INTERP = 3,
		PT_NOTE = 4,
		PT_SHLIB = 5,
		PT_PHDR = 6,
		PT_LOPROC = 0x70000000,
		PT_HIPROC = 0x7FFFFFFF
	} elf_p_type;

	Elf64_Off  p_offset;
	Elf64_Addr p_vaddr;
	Elf64_Addr p_paddr;
	Elf64_Word p_filesz;
	Elf64_Word p_memsz;
	Elf64_Word p_flags;
	Elf64_Word p_align;
};

MIPS64_API Mips64Status mips64_load_elf(Mips64Emulator* emulator, const void* data, size_t size);