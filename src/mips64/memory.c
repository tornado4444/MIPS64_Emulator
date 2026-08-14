#include "mips64/memory.h"

void mips64_memory_init(Memory* mem) {
	if(mem == NULL) {
		printf("Error to init memory");
		return;
	}

	memset(mem, 0, sizeof(*mem));

	mem->memory[MAX_SIZE];
}

void mips64_memory_reset(Memory* mem) {
	if(mem == NULL) {
		printf("Error to reset memory");
		return;
	}

	memset(mem, 0, sizeof(*mem));

	mem->memory[MAX_SIZE];
}

// For read
uint8_t mips64_8bit_read(Memory* mem, uint64_t address) { 
	if(!validAddress(address, 1)) return 0;
	return mem->memory[address];
}

uint16_t mips64_16bit_read(Memory* mem, uint64_t address) {
	if(!validAddress(address, 2)) return 0;
	return (uint16_t)mem->memory[address] | 
		((uint16_t) mem->memory[address + 1] << 8);
}

uint32_t mips64_32bit_read(Memory* mem, uint64_t address) {
	if(!validAddress(address, 4)) return 0;
	return (uint32_t) mem->memory[address] | 
		((uint32_t) mem->memory[address + 1] << 8) | 
		((uint32_t) mem->memory[address + 2] << 16) | 
		((uint32_t) mem->memory[address + 3] << 24);
}

uint64_t mips64_64bit_read(Memory* mem, uint64_t address) {
	if(!validAddress(address, 8)) return 0;
	uint64_t value = 0;
	for(int i = 0; i < 8; i++) {
		value |= ((uint64_t)mem->memory[address+i] << (i * 8));
	}

	return value;
}

// For write
uint8_t mips64_8bit_write() {
	
}

uint16_t mips64_16bit_write() {
	
}

uint32_t mips64_32bit_write() {
	
}

uint64_t mips64_64bit_write() {
	
}
