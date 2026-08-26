#include <string.h>

#include "mips64/cpu.h"
#include "mips64/decode.h"

// TODO IN FUTURE RELEASE ELF AND JIT!!!!!!!
void mips64_cpu_init(Mips64CPU* cpu, uint64_t reset_pc) {
	if (cpu == NULL) {
		return;
	}

	memset(cpu, 0, sizeof(*cpu));

	cpu->pc = reset_pc;
	cpu->gpr[0] = UINT64_C(0);
}

void mips64_cpu_reset(Mips64CPU* cpu, uint64_t reset_pc) {
	if (cpu == NULL) {
		return;
	}

	memset(cpu, 0, sizeof(*cpu));

	cpu->pc = reset_pc;
	cpu->gpr[0] = UINT64_C(0);
}

Mips64Status mips64_cpu_mips_get_gpr(const Mips64CPU* cpu, uint32_t index, uint64_t* out_value) {
	if (cpu == NULL || out_value == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (index >= MIPS64_GPR_COUNT) {
		return MIPS64_STATUS_INVALID_REGISTER;
	}

	if (index == 0u) {
		*out_value = UINT64_C(0);
	}
	else {
		*out_value = cpu->gpr[index];
	}

	return MIPS64_STATUS_OK;
}

Mips64Status mips64_cpu_set_gpr(Mips64CPU* cpu, uint32_t index, uint64_t value)
{
	if (cpu == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (index >= MIPS64_GPR_COUNT) {
		return MIPS64_STATUS_INVALID_REGISTER;
	}

	if (index != 0u) {
		cpu->gpr[index] = value;
	}

	cpu->gpr[0] = UINT64_C(0);

	return MIPS64_STATUS_OK;
}

Mips64Status mips64_cpu_get_pc(const Mips64CPU* cpu, uint64_t* out_pc) {
	if (cpu == NULL || out_pc == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	*out_pc = cpu->pc;

	return MIPS64_STATUS_OK;
}

Mips64Status mips64_cpu_set_pc(Mips64CPU* cpu, uint64_t pc) {
	if (cpu == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if ((pc & UINT64_C(3)) != UINT64_C(0)) {
		return MIPS64_STATUS_UNALIGNED_PC;
	}


	cpu->pc = pc;
	return MIPS64_STATUS_OK;
}
