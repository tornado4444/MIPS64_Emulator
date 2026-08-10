#pragma once

#include "mips64/core.h"

#define MAX_SIZE 1024u
#define BLOCK_SIZE 16u

/* TODO FOR MEMORY
	1) LoadMemory, StoreMemory();
	2) Little-Endian, Big-Endian;(BigEndianMem)
	3) memory_size = 64 * 1024 * 1024
*/

// FOR MEMORY ~1 MB
typedef struct Memory {
	uint8_t memory[MAX_SIZE];
} Memory;