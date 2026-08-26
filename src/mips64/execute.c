#include "mips64/execute.h"

static uint64_t sign_extension(uint16_t value)
{
	return (uint64_t)(int64_t)(int16_t)value;
}

static Mips64Status mips64_execute_nop(Mips64CPU* cpu, const Mips64Decoded* instruction) { //  0x000000
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	cpu->pc += UINT64_C(4);
	return MIPS64_STATUS_OK;
} 

static Mips64Status mips64_execute_daddiu(Mips64CPU* cpu, const Mips64Decoded* instruction) { // Operation: GPR[rd] <- GPR[rs] + GPR[rt];
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

static Mips64Status mips64_execute_daddu(Mips64CPU* cpu, const Mips64Decoded* instruction) { // Operation: GPR[rt]  GPR[rs] + sign_extend(immediate)
	uint64_t result;

	result = cpu->gpr[instruction->rs] + cpu->gpr[instruction->rt];

	if (instruction->rd != 0) {
		cpu->gpr[instruction->rd] = result;
	}
	
	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
} 

static Mips64Status mips64_execute_special(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	switch (instruction->function) {
	case MIPS64_FUNCT_DADDU:
		return mips64_execute_daddu(cpu, instruction);
	default:
		printf("Error or incorrect opcode: ", instruction->function);
		return MIPS64_STATUS_NOT_IMPLEMENTED;
	}
}

Mips64Status mips64_cpu_execute(Mips64CPU* cpu, const Mips64Decoded* raw_instruction) {
	if (cpu == NULL || raw_instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (raw_instruction->raw == UINT32_C(0)) {
		return mips64_execute_nop(cpu, raw_instruction);
	}

	switch (raw_instruction->opcode) {
	case MIPS64_OPCODE_SPECIAL:
		return mips64_execute_special(cpu, raw_instruction);

	case MIPS64_OPCODE_DADDIU:
		return mips64_execute_daddiu(cpu, raw_instruction);

	default:
		return MIPS64_STATUS_NOT_IMPLEMENTED;
	}
}
