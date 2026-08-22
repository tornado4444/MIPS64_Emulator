#pragma once

#include "mips64/core.h"

// RELEASE THE CPU MIPS64	
typedef struct Mips64CPU {
	uint64_t gpr[MIPS64_GPR_COUNT];

	uint64_t pc; // Program Count register
	/* TODO NEED BEEN RELEASE ON FUTURE FOR
	* uint64_t hi; // Multiply and Divide register higher result(Removed in Release 6, 
	* I'll implement it anyway because I'll be making several releases, including Release 2.)
	* uint64_t lo; // Multiply and Divide register lower result(Removed in Release 6, 
	* but I need I'll implement it anyway because I'll be making several releases, including Release 2.)
	*/
} Mips64CPU;

void mips64_cpu_init(Mips64CPU* cpu, uint64_t reset_pc);
void mips64_cpu_reset(Mips64CPU* cpu, uint64_t reset_pc);

Mips64Status mips64_cpu_mips_get_gpr(const Mips64CPU* cpu, uint32_t index, uint64_t* out_value);
Mips64Status mips64_cpu_set_gpr(Mips64CPU* cpu, uint32_t index, uint64_t value);
Mips64Status mips64_cpu_get_pc(const Mips64CPU* cpu, uint64_t* out_pc);
Mips64Status mips64_cpu_set_pc(Mips64CPU* cpu, uint64_t pc);

