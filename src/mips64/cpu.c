#include "mips64/cpu.h"
#include <string.h>

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

Mips64Status mips64_cpu_set_gpr(Mips64CPU* cpu, uint32_t index, uint64_t value) {
	if (cpu == NULL || value == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (index >= MIPS64_GPR_COUNT) {
		return MIPS64_STATUS_INVALID_REGISTER;
	}

	if (index != 0u) {
		value = UINT64_C(0);
	}
	else {
		cpu->gpr[index] = value;
	}

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

	if (pc & UINT64_C(3) != UINT64_C(0)) {
		return MIPS64_STATUS_UNALIGNED_PC;
	}

	cpu->pc = pc;

	return MIPS64_STATUS_OK;
}

void decode_instruction(uint32_t raw, Mips64Decoded* out_instruction) {
	out_instruction->raw = raw;

	out_instruction->opcode = (uint8_t)(raw >> 26) & UINT32_C(0x3F);
	out_instruction->rs = (uint8_t)(raw >> 21) & UINT32_C(0x1F);
	out_instruction->rt = (uint8_t)(raw >> 16) & UINT32_C(0x1F);
	out_instruction->rd = (uint8_t)(raw >> 11) & UINT32_C(0x1F);
	out_instruction->shift_amount = (uint8_t)(raw >> 6) & UINT32_C(0x1F);
	out_instruction->function = (uint8_t)(raw & UINT32_C(0x3F));
	out_instruction->immediate = (uint16_t)(raw & UINT32_C(0xFFF));
	out_instruction->target = raw & UINT32_C(0x03FFFFFFFF);
}

uint64_t sign_extension(uint16_t value) {
	return (uint64_t)(uint64_t)(uint16_t)value;
}

Mips64Status mips64_execute_nop(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (instruction == UINT32_C(0)) {
		cpu->pc += UINT64_C(4);
		return MIPS64_STATUS_OK;
	}
}

Mips64Status mips64_execute_daddiu(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	uint64_t source;
	uint64_t immediate;
	
	uint64_t result;
	
	source = cpu->gpr[instruction->rs];

	immediate = sign_extension(instruction->immediate);

	result = source + immediate;

	if (instruction->rt != 0u) {
		cpu->gpr[instruction->rt] = result;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

Mips64Status mips64_execute_daddu(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	uint64_t result;

	result = cpu->gpr[instruction->rs] + cpu->gpr[instruction->rt];

	if (instruction->rd != 0) {
		cpu->gpr[instruction->rd] = result;
	}
	
	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}