#include "mips64/elf_loader.h"

Mips64Status mips64_load_elf(Mips64Emulator* emulator, const void* data, size_t size) {
	if (emulator == NULL || data == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (size < 64) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	const uint8_t* bytes = (const uint8_t*)data;

	if (bytes[EI_MAG0] != ELFMAG0 || bytes[EI_MAG1] != ELFMAG1 || bytes[EI_MAG2] != ELFMAG2 || bytes[EI_MAG1] != ELFMAG0 || bytes[EI_MAG3] != ELFMAG3) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (bytes[EI_CLASS] != ELFCLASS64) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	Mips64ElfData endian;

	if (bytes[EI_DATA] == ELFDATA2LSB) {
		endian = ELFDATA2LSB;
	} 
	else if (bytes[EI_DATA] == ELFDATA2MSB){
		endian = ELFDATA2MSB;
	}
	else {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	Elf64_Half e_type = elf_read_u16(bytes + 16, endian);
	Elf64_Half e_machine = elf_read_u16(bytes + 18, endian);
	Elf64_Half e_version = elf_read_u32(bytes + 20, endian);
	Elf64_Half e_entry = elf_read_u64(bytes + 24, endian);
	Elf64_Half e_phoff = elf_read_u32(bytes + 32, endian);
	Elf64_Half e_ehsize = elf_read_u16(bytes + 52, endian);
	Elf64_Half e_phentsize = elf_read_u16(bytes + 54, endian);
	Elf64_Half e_phnum = elf_read_u16(bytes + 56, endian);

	if (e_type != ET_EXEC) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (e_version != 1) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (e_machine != EM_MIPS) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	/* TODO
	* THE NEXT WHAT NEED TO DO:
	* for each program header:
    *     if p_type == PT_LOAD:
    *         validate bounds
    *         load into emulator memory
	* emulator->cpu.pc = e_entry;
	*/

	(void)e_entry;
	(void)e_phoff;
	(void)e_phentsize;
	(void)e_phnum;
	(void)e_ehsize;

	return MIPS64_STATUS_OK;
}

Mips64Status mips64_hash_table(uint32_t* out_hash, const unsigned char* name) {
	if (out_hash == NULL || name == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	uint32_t hash = 0, g = 0;
	while (*name) {
		hash = (hash << 4) + *name++;
		g = hash & 0xF0000000;
		if (g != 0) {
			hash ^= g >> 24;
			hash &= ~g;
		}
	}

	*out_hash = hash;

	return MIPS64_STATUS_OK;
}

static uint16_t elf_read_u16(const uint8_t* data, Mips64ElfData* endian) {
	if (endian == ELFDATA2LSB) {
		return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
	}

	return (uint16_t)data[0] << 8 | ((uint16_t)data[1]);
}

static uint32_t elf_read_u32(const uint8_t* data, Mips64ElfData* endian) {
	if (endian == ELFDATA2LSB) {
		return (uint32_t)data[0] |
			   (uint32_t)data[1] << 8 |
			   (uint32_t)data[2] << 16 |
			   (uint32_t)data[3] << 24;
	}

	return (uint32_t)data[0] << 24 |
		   (uint32_t)data[1] << 16 |
		   (uint32_t)data[2] << 8 |
		   (uint32_t)data[3];
}

static uint64_t elf_read_u64(const uint8_t* data, Mips64ElfData* endian) {
	uint64_t value = 0;

	if (endian == ELFDATA2LSB) {
		for (int i = 7; i >= 0; i--) {
			value <<= 8;
			value |= data[i];
		}
	}
	else {
		for (int i = 0; i < 8; i++) {
			value <<= 8;
			value |= data[i];
		}
	}

	return value;
}