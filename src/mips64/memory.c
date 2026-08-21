#include "mips64/memory.h"

// Bounds checking
static bool mips64_bounds_checking(uint64_t address, uint64_t data_size) {
	if(data_size == 0) return false;

	if(address > UINT64_MAX - data_size) return false;

	if(address + data_size > MAX_SIZE) return false;

    	return true;
}

// Alignment checking
static bool mips64_alignment_checking(uint64_t address, uint64_t data_size) {
	/*
	 * Mask of the least significant bits:
	 * - for 2 bytes: mask 1(01b);
	 * - for 4 bytes: mask 3(11b);
	 * - for 8 bytes: mask 7(111b);
	 * */
	switch(data_size) {
		case 1:
			return true;
		case 2:
			return (address & UINT64_C(0x1)) == 0;
		case 4:
			return (address & UINT64_C(0x3)) == 0;
		case 8:
			return (address & UINT64_C(0x7)) == 0;
		default:
			return false;
	}
}

Mips64MemoryStatus mips64_memory_init(Memory* mem, Mips64Endian endian) {
	if(mem == NULL) {
		printf("Error to init memory");
		return MIPS64_MEMORY_NULL_POINTER;
	}

	memset(mem->memory, 0, sizeof(*mem));
	return MIPS64_MEMORY_OK;
}

Mips64MemoryStatus mips64_memory_reset(Memory* mem) {
	if(mem == NULL) {
		printf("Error to reset memory");
		return MIPS64_MEMORY_NULL_POINTER;
	}
	/*
	 * Reset RAM but preserve configuration.
	 * Including current endian mode.
	 * */
	memset(mem->memory, 0, sizeof(*mem));
	return MIPS64_MEMORY_OK;
}

// For read
Mips64MemoryStatus mips64_memory_read8(const Memory* mem, uint64_t address, uint8_t* out_value) {  
	if(!mips64_bounds_checking(address, 1)) {
		fprinf(stderr, "Error: Out of bounds memory! Reading 1 byte at address 0x%016llX\n", address);
		return 0;
	}
	return mem->memory[address];
}

Mips64MemoryStatus mips64_memory_read16(const Memory* mem, uint64_t address, uint16_t* out_value) {
	if(!mips64_bounds_checking(address, 2)) {
		fprinf(stderr, "Error: Out of bounds memory! Reading 2 bytes at address 0x%016llX\n", address);
		return 0;
	}
	return (uint16_t)mem->memory[address] | 
		((uint16_t) mem->memory[address + 1] << 8);
}

Mips64MemoryStatus mips64_memory_read32(const Memory* mem, uint64_t address, uint32_t* out_value){
	if(!mips64_bounds_checking(address, 4)) {
		fprinf(stderr, "Error: Out of bounds memory! Reading 4 bytes at address 0x%016llX\n", address);
		return 0;
	}
	return (uint32_t) mem->memory[address] | 
		((uint32_t) mem->memory[address + 1] << 8) | 
		((uint32_t) mem->memory[address + 2] << 16) | 
		((uint32_t) mem->memory[address + 3] << 24);
}

Mips64MemoryStatus mips64_memory_read64(const Memory* mem, uint64_t address, uint64_t* out_value){
	if(!mips64_bounds_checking(address, 8)) {
		fprinf(stderr, "Error: Out of bounds memory! Reading 8 bytes at address 0x%016llX\n", address);
		return 0;
	}
	uint64_t value = 0;
	for(int i = 0; i < 8; i++) {
		value |= ((uint64_t)mem->memory[address+i] << (i * 8));
	}

	return value;
}

// For write
Mips64MemoryStatus mips64_memory_write8(const Memory* mem, uint64_t address, uint8_t* out_value) {
	if(!mips64_bounds_checking(address, 1)) {
		fprinf(stderr, "Error: Out of bounds memory! Writing 1 byte at address 0x%016llX\n", address);
		return 0;
	}
	mem->memory[address] = value
}

Mips64MemoryStatus mips64_memory_write16(const Memory* mem, uint64_t address, uint16_t* out_value){
	if(!mips64_bounds_checking(address, 2)) {
		fprinf(stderr, "Error: Out of bounds memory! Writing 2 bytes at address 0x%016llX\n", address);
		return 0;
	}
	mem->memory[address] = value & 0xFF;
	mem->memory[address + 1] = (value >> 8) & 0xFF;
}

Mips64MemoryStatus mips64_memory_write32(const Memory* mem, uint64_t address, uint32_t* out_value){
	if(!mips64_bounds_checking(address, 4)) {
		fprinf(stderr, "Error: Out of bounds memory! Writing 4 bytes at address 0x%016llX\n", address);
		return 0;
	}
	mem->memory[address] = value & 0xFF;
	mem->memory[address + 1] = (value >> 8) & 0xFF;
	mem->memory[address + 2] = (value >> 16) & 0xFF;
	mem->memory[address + 3] = (value >> 24) & 0xFF;
}

Mips64MemoryStatus mips64_memory_write64(const Memory* mem, uint64_t address, uint64_t* out_value) {
	if(!mips64_bounds_checking(address, 8)) {
		fprinf(stderr, "Error: Out of bounds memory! Writing 8 bytes at address 0x%016llX\n", address);
		return 0;
	}
	for(int i = 0; i < 8; i++) {
		mem->memory[address+i] = (value >> (i * 8)) & 0xFF;
	}
}
