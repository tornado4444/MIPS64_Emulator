#pragma once

#include "mips64/core.h"
#include "mips64/cpu.h"
#include "mips64/memory.h"
#include "mips64/decode.h" 

Mips64Status mips64_cpu_execute(Mips64CPU* cpu, const Mips64Decoded* raw_instruction);

