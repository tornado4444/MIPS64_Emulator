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
	if(mem == NULL || out_value == NULL) {
		return MIPS64_MEMORY_NULL_POINTER;
	}

	if(!mips64_bound_checking(address, 1)) {
		return MIPS64_MEMORY_OUT_OF_BOUNDS;
	}
	
	*out_value = mem->memory[address];
	return MIPS64_MEMORY_OK;	
}

Mips64MemoryStatus mips64_memory_read16(const Memory* mem, uint64_t address, uint16_t* out_value) {
	if(mem == NULL || out_value == NULL) {
		return MIPS64_MEMORY_NULL_POINTER;
	}

	if(!mips64_bound_checking(address, 2)) {
		return MIPS64_MEMORY_OUT_OF_BOUNDS;
	}

	if(!mips64_memory_alignment_checking(address, 2)) {
		return MIPS64_MEMORY_MISALIGNED;
	}

	if(mem->endian == MIPS64_ENDIAN_LITTLE) {
		*out_value = ((uint16_t) mem->memory[address]) |
			     ((uint16_t) mem->memory[address + 1] << 8);
	} else {
		*out_value = ((uint16_t) mem->memory[address] << 8 |
			     ((uint16_t) mem->memory[address + 1]);
	}
	
	return MIPS64_MEMORY_OK;

}

Mips64MemoryStatus mips64_memory_read32(const Memory* mem, uint64_t address, uint32_t* out_value){
	if(mem == NULL || out_value == NULL) {
		return MIPS64_MEMORY_NULL_POINTER;
	}

	if(!mips64_bound_checking(address, 4)) {
		return MIPS64_MEMORY_OUT_OF_BOUNDS;
	}

	if(!mips64_memory_alignment_checking(address, 4)) {
		return MIPS64_MEMORY_MISALIGNED;
	}

	if(mem->endian == MIPS64_ENDIAN_LITTLE) {
		*out_value = ((uint32_t) mem->memory[address])           |
			     ((uint32_t) mem->memory[address + 1] << 8)  |
			     ((uint32_t) mem->memory[address + 2] << 16) |
			     ((uint32_t) mem->memory[address + 3] << 24);
	} else {
		*out_value = (uint32_t) mem->memory[address] << 24       |
			     ((uint32_t) mem->memory[address + 1] << 16) |
			     ((uint32_t) mem->memory[address + 2] << 8)  |
			     ((uint32_t) mem->memory[address + 3]);
	}
	
	return MIPS64_MEMORY_OK;
	
}

Mips64MemoryStatus mips64_memory_read64(const Memory* mem, uint64_t address, uint64_t* out_value){
	if(mem == NULL || out_value == NULL) {
		return MIPS64_MEMORY_NULL_POINTER;
	}

	if(!mips64_bound_checking(address, 8)) {
		return MIPS64_MEMORY_OUT_OF_BOUNDS;
	}

	if(!mips64_memory_alignment_checking(address, 8)) {
		return MIPS64_MEMORY_MISALIGNED;
	}


	uint64_t value = 0;
	if(mem->endian == MIPS64_ENDIAN_LITTLE) {
		for (uint32_t i = 0; i < 8; ++i) {
			value |= ((uint64_t) mem->memory[address + i]) << (i * 8u));
		}
	} else {
		for(uint32_t i = 0; i < 8; ++i) {
			value |= ((uint64_t) mem->memory[address + i]) << ((7u - i) * 8u);
		}
	}
	
	*out_value = value;

	return MIPS64_MEMORY_OK;
}

// For write
Mips64MemoryStatus mips64_memory_write8(const Memory* mem, uint64_t address, uint8_t value) {
	if(mem == NULL) {
		return MIPS64_NULL_POINTER;
	}	

	if(!mips64_bound_checking(address, 1)) {
		return MIPS64_MEMORY_OUT_OF_BOUNDS;
	}
	
	mem->memory[address] = value;
	return MIPS64_MEMORY_OK;
}

Mips64MemoryStatus mips64_memory_write16(const Memory* mem, uint64_t address, uint16_t value){
	if(mem == NULL) {
		return MIPS64_NULL_POINTER;
	}	

	if(!mips64_bound_checking(address, 2)) {
		return MIPS64_MEMORY_OUT_OF_BOUNDS;
	}
	
	if(!mips64_memory_alignment_checking(address, 2)) {
		return MIPS64_MEMORY_MISALIGNED;
	}
	
	if(mem->endian == MIPS64_LITTLE_ENDIAN) {
		mem->memory[address] = (uint8_t) (value & UINT16_C(0x00FF));
		mem->memory[address + 1] = (uint8_t) ((value >> 8) & UINT16_C(0x00FF));
	} else {
		mem->memory[address] = (uint8_t) ((value >> 8) & UINT16_C(0x00FF));
		mem->memory[address + 1] = (uint8_t) (value & UINT16_C(0x00FF));
	}

	return MIPS64_MEMORY_OK;
}

Mips64MemoryStatus mips64_memory_write32(const Memory* mem, uint64_t address, uint32_t value){
	if(mem == NULL) {
		return MIPS64_NULL_POINTER;
	}	

	if(!mips64_bound_checking(address, 4)) {
		return MIPS64_MEMORY_OUT_OF_BOUNDS;
	}
	
	if(!mips64_memory_alignment_checking(address, 4)) {
		return MIPS64_MEMORY_MISALIGNED;
	}
	
	if(mem->endian == MIPS64_LITTLE_ENDIAN) {
		mem->memory[address] = (uint16_t) (value & UINT32_C(0x000000FF));
		mem->memory[address + 1] = (uint16_t) ((value >> 8) & UINT32_C(0x000000FF));
		mem->memory[address + 2] = (uint16_t) ((value >> 16) & UINT32_C(0x000000FF));
		mem->memory[address + 3] = (uint16_t) ((value >> 24) & UINT32_C(0x000000FF));
	} else {
		mem->memory[address] = (uint16_t) ((value >> 24) & UINT32_C(0x000000FF));
		mem->memory[address + 1] = (uint16_t) ((value >> 16) & UINT32_C(0x000000FF));
		mem->memory[address + 2] = (uint16_t) ((value > 8) & UINT32_C(0x000000FF));
		mem->memory[address + 3] = (uint16_t) (value & UINT32_C(0x000000FF));
	}


	return MIPS64_MEMORY_OK;
}

Mips64MemoryStatus mips64_memory_write64(const Memory* mem, uint64_t address, uint64_t value) {
	if(mem == NULL) {
		return MIPS64_NULL_POINTER;
	}

	if(!mips64_bound_checking(address, 8)) {
		return MIPS64_MEMORY_OUT_OF_BOUNDS;
	}
	
	if(!mips64_memory_alignment_checking(address, 8)) {
		return MIPS64_MEMORY_MISALIGNED;
	}
	
	if(mem->endian == MIPS64_LITTLE_ENDIAN) {
		for(uint32_t i = 0; i < 8, ++i) {
			mem->memory[address + i] = (uint8_t) (value >> ((7u - i) * 8u)) & UINT64_C(0xFF);
		}
	} else {
		for(uint32_t i = 0; i < 8, ++i) {
			mem->memory[address + i] = (uint8_t) (value >> ((7u - i) * 8u)) & UINT64_C(0xFF);
		}

	}

	return MIPS64_MEMORY_OK;
}
