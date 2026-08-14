#pragma once

#include "mips64/core.h"

// memory_size = 64 * 1024 * 1024
#define MAX_SIZE (64u * 1024u * 1024u)
#define BLOCK_SIZE 16u

/* TODO FOR MEMORY
*	1) LoadMemory, StoreMemory();
*	2) Little-Endian, Big-Endian;(BigEndianMem)
*	3) memory_size = 64 * 1024 * 1024
*	4) In future release MMU
*/

// FOR MEMORY ~64 MB
typedef struct Memory {
	uint8_t memory[MAX_SIZE];
} Memory;


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
uint8_t mips64_8bit_read();
uint16_t mips64_16bit_read();
uint32_t mips64_32bit_read();
uint64_t mips64_64bit_read();

// For write
uint8_t mips64_8bit_write();
uint16_t mips64_16bit_write();
uint32_t mips64_32bit_write();
uint64_t mips64_64bit_write();

