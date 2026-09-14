#pragma once

#include "mips64/core.h"

// RELEASE THE CPU MIPS64	
typedef struct Mips64CPU {
	uint64_t gpr[MIPS64_GPR_COUNT];

	uint64_t pc; // Program Count register
    /*
     * Architectural HI/LO registers used by
     * older MIPS64 ISA releases.
     *
     * Kept in the common CPU state even for
     * releases where they are not architecturally used.
     */
    uint64_t hi; // Multiply and Divide register higher result(lower Release 6)
    uint64_t lo; // LO - Multiply and Divide register lower result(lower Release 6)
} Mips64CPU;

Mips64Status mips64_cpu_reset(Mips64CPU* cpu, uint64_t reset_pc);
Mips64Status mips64_cpu_mips_get_gpr(const Mips64CPU* cpu, uint32_t index, uint64_t* out_value);
Mips64Status mips64_cpu_set_gpr(Mips64CPU* cpu, uint32_t index, uint64_t value);
Mips64Status mips64_cpu_get_pc(const Mips64CPU* cpu, uint64_t* out_pc);
Mips64Status mips64_cpu_set_pc(Mips64CPU* cpu, uint64_t pc);
