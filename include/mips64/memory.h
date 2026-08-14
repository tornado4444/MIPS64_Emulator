#pragma once

#include "mips64/core.h"

// memory_size = 64 * 1024 * 1024
#define MAX_SIZE (64u * 1024u * 1024u)
#define BLOCK_SIZE 16u

/* TODO FOR MEMORY
*	1) LoadMemory, StoreMemory();
*	2) Little-Endian, Big-Endian;(BigEndianMem)
*	3) In future release MMU
*/

// FOR MEMORY ~64 MB
typedef struct Memory {
	uint8_t memory[MAX_SIZE];
} Memory;


// Checking for out of bounds
bool validAddress(uint64_t address, uint64_t size) {
	return (address + size <= MAX_SIZE);
}

/* TODO
 * 
 * Then:
 * Release following memory access types:
 * - Uncached;
 * - Cached;
 * - Uncached Accelerated.
 * Also later release:
 * - MMU/CCA
 * - CP0-related memory attributes
*/
void mips64_memory_init(Memory* mem);
void mips64_memory_reset(Memory* mem);

// For read
uint8_t mips64_8bit_read(Memory* mem, uint64_t address); // Byte = 8 bit
uint16_t mips64_16bit_read(Memory* mem, uint64_t address); // Halfword = 16 bit
uint32_t mips64_32bit_read(Memory* mem, uint64_t address); // Word = 32 bit
uint64_t mips64_64bit_read(Memory* mem, uint64_t address); // Doubleword = 64 bit

// For write
uint8_t mips64_8bit_write(Memory* mem, uint64_t address, uint8_t value);
uint16_t mips64_16bit_write(Memory* mem, uint64_t address, uint8_t value);
uint32_t mips64_32bit_write(Memory* mem, uint64_t address, uint8_t value);
uint64_t mips64_64bit_write(Memory* mem, uint64_t address, uint8_t value);

