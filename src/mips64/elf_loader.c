#include "mips64/elf_loader.h"

Mips64Status mips64_load_elf(Mips64Emulator* emulator, const void* data, size_t size) {
	if (emulator == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

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