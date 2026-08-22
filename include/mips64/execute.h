#pragma once

#include "mips64/core.h"
#include "mips64/cpu.h"
#include "mips64/memory.h"

static Mips64Status mips64_cpu_execute(Mips64CPU* cpu, uint32_t raw_instruction);
static Mips64Status mips64_execute_special(Mips64CPU* cpu, const Mips64Decoded* instruction);
static Mips64Status mips64_execute_nop(Mips64CPU* cpu, const Mips64Decoded* instruction); //  0x000000
static Mips64Status mips64_execute_daddiu(Mips64CPU* cpu, const Mips64Decoded* instruction); // Operation: GPR[rd] <- GPR[rs] + GPR[rt];
static Mips64Status mips64_execute_daddu(Mips64CPU* cpu, const Mips64Decoded* instruction); // Operation: GPR[rt]  GPR[rs] + sign_extend(immediate) 
