#include "mips64/memory.h"

// Bounds checking
bool mips64_bounds_checking(uint64_t address, uint64_t data_size) {
	if(data_size == 0) return false;

	if(address > UINT64_MAX - data_size) return false;

	if(address + data_size > MAX_SIZE) return false;

    	return true;
}

// Alignment checking
bool mips64_alignment_checking(uint64_t address, uint64_t data_size) {
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
			return (address & 0x1u) == 0;
		case 4:
			return (address & 0x3u) == 0;
		case 8:
			return (address & 0x7u) == 0;
		default:
			return false;
	}
}

void mips64_memory_init(Memory* mem) {
	if(mem == NULL) {
		printf("Error to init memory");
		return;
	}

	memset(mem, 0, sizeof(*mem));
}

void mips64_memory_reset(Memory* mem) {
	if(mem == NULL) {
		printf("Error to reset memory");
		return;
	}

	memset(mem, 0, sizeof(*mem));
}

// For read
uint8_t mips64_8bit_read(Memory* mem, Checking* cpu, uint64_t address) { 
	if(!mips64_bounds_checking(address, 1)) {
		fprinf(stderr, "Error: Out of bounds memory! Reading 1 byte at address 0x%016llX\n", address);
		return 0;
	}
	return mem->memory[address];
}

uint16_t mips64_16bit_read(Memory* mem, uint64_t address) {
	if(!mips64_bounds_checking(address, 2)) {
		fprinf(stderr, "Error: Out of bounds memory! Reading 2 bytes at address 0x%016llX\n", address);
		return 0;
	}
	return (uint16_t)mem->memory[address] | 
		((uint16_t) mem->memory[address + 1] << 8);
}

uint32_t mips64_32bit_read(Memory* mem, uint64_t address) {
	if(!mips64_bounds_checking(address, 4)) {
		fprinf(stderr, "Error: Out of bounds memory! Reading 4 bytes at address 0x%016llX\n", address);
		return 0;
	}
	return (uint32_t) mem->memory[address] | 
		((uint32_t) mem->memory[address + 1] << 8) | 
		((uint32_t) mem->memory[address + 2] << 16) | 
		((uint32_t) mem->memory[address + 3] << 24);
}

uint64_t mips64_64bit_read(Memory* mem, uint64_t address) {
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
uint8_t mips64_8bit_write(Memory* mem, uint64_t address, uint8_t value) {
	if(!mips64_bounds_checking(address, 1)) {
		fprinf(stderr, "Error: Out of bounds memory! Writing 1 byte at address 0x%016llX\n", address);
		return 0;
	}
	mem->memory[address] = value
}

uint16_t mips64_16bit_write(Memory* mem, uint64_t address, uint8_t value) {
	if(!mips64_bounds_checking(address, 2)) {
		fprinf(stderr, "Error: Out of bounds memory! Writing 2 bytes at address 0x%016llX\n", address);
		return 0;
	}
	mem->memory[address] = value & 0xFF;
	mem->memory[address + 1] = (value >> 8) & 0xFF;
}

uint32_t mips64_32bit_write() {
	if(!mips64_bounds_checking(address, 4)) {
		fprinf(stderr, "Error: Out of bounds memory! Writing 4 bytes at address 0x%016llX\n", address);
		return 0;
	}
	mem->memory[address] = value & 0xFF;
	mem->memory[address + 1] = (value >> 8) & 0xFF;
	mem->memory[address + 2] = (value >> 16) & 0xFF;
	mem->memory[address + 3] = (value >> 24) & 0xFF;
}

uint64_t mips64_64bit_write() {
	if(!mips64_bounds_checking(address, 8)) {
		fprinf(stderr, "Error: Out of bounds memory! Writing 8 bytes at address 0x%016llX\n", address);
		return 0;
	}
	for(int i = 0; i < 8; i++) {
		mem->memory[address+i] = (value >> (i * 8)) & 0xFF;
	}
}
