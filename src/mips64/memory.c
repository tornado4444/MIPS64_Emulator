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
uint8_t mips64_8bit_read() {
	
}

uint16_t mips64_16bit_read() {
	
}

uint32_t mips64_32bit_read() {
	
}

uint64_t mips64_64bit_read() {
	
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
