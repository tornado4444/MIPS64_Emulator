#pragma once

#include <stdint.h>
#include <stddef.h>

#include "mips64/core.h"

#define EI_NIDENT			16

#define ELFMAG0				0x7F
#define ELFMAG1				'E'
#define ELFMAG2				'L'
#define ELFMAG3				'F'

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;

typedef uint16_t Elf64_Half;

typedef uint32_t Elf64_Word;
typedef int32_t  Elf64_Sword;

typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;

typedef enum Mips64ElfIdentIndex {
    EI_MAG0       = 0,
    EI_MAG1       = 1,
    EI_MAG2       = 2,
    EI_MAG3       = 3,

    EI_CLASS      = 4,
    EI_DATA       = 5,
    EI_VERSION    = 6,
    EI_OSABI      = 7,
    EI_ABIVERSION = 8,
    EI_PAD        = 9
} Mips64ElfIdentIndex;

typedef enum Mips64ElfClass {
    ELFCLASSNONE = 0,
    ELFCLASS32   = 1,
    ELFCLASS64   = 2
} Mips64ElfClass;

typedef enum Mips64ElfData {
    ELFDATANONE = 0,
    ELFDATA2LSB = 1,
    ELFDATA2MSB = 2
} Mips64ElfData;

typedef enum Mips64ElfType {
    ET_NONE = 0,
    ET_REL  = 1,
    ET_EXEC = 2,
    ET_DYN  = 3,
    ET_CORE = 4
} Mips64ElfType;

typedef enum Mips64ElfMachine {
    EM_MIPS = 8
} Mips64ElfMachine;

typedef struct Elf64_Ehdr {
    unsigned char e_ident[EI_NIDENT];

    Elf64_Half e_type;
    Elf64_Half e_machine;

    Elf64_Word e_version;

    Elf64_Addr e_entry;

    Elf64_Off e_phoff;
    Elf64_Off e_shoff;

    Elf64_Word e_flags;

    Elf64_Half e_ehsize;

    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;

    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;

    Elf64_Half e_shstrndx;
} Elf64_Ehdr;

typedef enum Mips64ElfProgramType {
    PT_NULL     = 0,
    PT_LOAD     = 1,
    PT_DYNAMIC  = 2,
    PT_INTERP   = 3,
    PT_NOTE     = 4,
    PT_SHLIB    = 5,
    PT_PHDR     = 6
} Mips64ElfProgramType;

typedef enum Mips64ElfProgramFlags {
    PF_X = 0x1,
    PF_W = 0x2,
    PF_R = 0x4
} Mips64ElfProgramFlags;

typedef struct Elf64_Phdr {
    Elf64_Word  p_type;
    Elf64_Word  p_flags;

    Elf64_Off   p_offset;

    Elf64_Addr  p_vaddr;
    Elf64_Addr  p_paddr;

    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;

    Elf64_Xword p_align;
} Elf64_Phdr;

MIPS64_API Mips64Status mips64_load_elf(Mips64Emulator* emulator, const void* data, size_t size);

// ---------------------------------HASH TABLE-------------------------------------
MIPS64_API Mips64Status mips64_hash_table(uint32_t* out_hash, const unsigned char* name);

static uint16_t elf_read_u16(const uint8_t* data, Mips64ElfData* endian);
static uint32_t elf_read_u32(const uint8_t* data, Mips64ElfData* endian);
static uint64_t elf_read_u64(const uint8_t* data, Mips64ElfData* endian);
