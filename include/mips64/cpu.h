#pragma once

#include "mips64/core.h"

// RELEASE THE CPU	
typedef struct CPU {
	int bit; 
	int8_t byte; // 8 bit, B
	int16_t halfword; // 16 bits, H
	int32_t Word; // 32 bits, W
	int64_t DoubleWord; // 64 bits, D
};