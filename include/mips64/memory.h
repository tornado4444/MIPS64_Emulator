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
static void LoadStore();
static void StoreMemory();

