#pragma once

#include "mips64/core.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// memory_size = 64 * 1024 * 1024
#define MAX_SIZE (64u * 1024u * 1024u)

/* TODO
 * Base memory implementation:
 * - Memory storage
 * - Bounds checking
 * - Byte / Halfword / Word / Doubleword access
 * - Little-Endian / Big-Endian
 * - Memory alignment rules
 *
 * Future:
 * - MMU
 * - Cached
 * - Uncached
 * - Uncached Accelerated
 * - CCA
 * - CP0-related memory attributes
 */

// FOR MEMORY ~64 MB
typedef struct Memory {
	uint8_t memory[MAX_SIZE];
	Mips64Endian endian; 
} Memory;

typedef enum Mips64MemoryStatus{
	MIPS64_MEMORY_OK = 0,
	MIPS64_MEMORY_NULL_POINTER,
	MIPS64_MEMORY_OUT_OF_BOUNDS,
	MIPS64_MEMORY_MISALIGNED
} Mips64Exception;

/*
 * FULL CHANGE THE FUNCTIONS BECAUSE WILL ALSO BE USING BIG ENDIAN. 
 * IF THERE WAS ONLY LITTLE ENDIAN, COULD HAVE USED WITH THE NORMAL FUNCTIONS, 
 * BUT MIPS64 HAS TWO DESCRIBED ENDIANS!!!
 * */

Mips64MemoryStatus mips64_memory_init(Memory* mem, Mips64Endian endian);
Mips64MemoryStatus mips64_memory_reset(Memory* mem);

// --READ--
Mips64MemoryStatus mips64_memory_read8(const Memory* mem, uint64_t address, uint8_t* out_value);    // Read Byte = 8 bit
Mips64MemoryStatus mips64_memory_read16(const Memory* mem, uint64_t address, uint16_t* out_value);  // Read Halfword = 16 bit
Mips64MemoryStatus mips64_memory_read32(const Memory* mem, uint64_t address, uint32_t* out_value);  // Read Word = 32 bit
Mips64MemoryStatus mips64_memory_read64(const Memory* mem, uint64_t address, uint64_t* out_value);  // Read Doubleword = 64 bit

// --WRITE--
Mips64MemoryStatus mips64_memory_write8(const Memory* mem, uint64_t address, uint8_t value);   // Write Byte = 8 bit
Mips64MemoryStatus mips64_memory_write16(const Memory* mem, uint64_t address, uint16_t value); // Write Halfword = 16 bit
Mips64MemoryStatus mips64_memory_write32(const Memory* mem, uint64_t address, uint32_t value); // Write Word = 32 bit
Mips64MemoryStatus mips64_memory_write64(const Memory* mem, uint64_t address, uint64_t value); // Write Doubleword = 64 bit

