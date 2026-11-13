#include <stdio.h>

#include "mips64/execute.h"

static uint64_t sign_extension(uint16_t value)
{
	return (uint64_t)(int64_t)(int16_t)value;
}

static uint64_t zero_extend(uint16_t zero_extend_value) {
	return (uint64_t)zero_extend_value;
}


static Mips64Status mips64_execute_nop(Mips64CPU* cpu, const Mips64Decoded* instruction) { //  0x000000
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	cpu->pc += UINT64_C(4);
	return MIPS64_STATUS_OK;
} 

// =================================================================OPCODE==============================
static Mips64Status mips64_execute_addi(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (!cpu->gpr[instruction->rs]) {
		UNPREDICTABLE;
	}

	uint64_t value = (uint64_t)(uint32_t)(uint32_t)cpu->gpr[instruction->rs];
	uint64_t immediate = sign_extension(instruction->immediate);

	uint64_t temp = value + immediate;
	if (temp > INT32_MAX || temp < INT32_MIN) {
		return MIPS64_STATUS_INTEGER_OVERFLOW;
	}

	if (instruction->rd != 0) {
		cpu->gpr[instruction->rd] = (uint64_t)(int64_t)(int32_t)temp;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_addiu(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (!cpu->gpr[instruction->rs]) {
		UNPREDICTABLE;
	}
	
	uint64_t value = (uint64_t)(uint32_t)(uint32_t)cpu->gpr[instruction->rs];

	uint64_t temp = value + sign_extension(instruction->immediate);

	if (instruction->rt != 0) {
		cpu->gpr[instruction->rt] = (uint64_t)(int64_t)(int32_t)temp;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);
	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_andi(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	cpu->gpr[instruction->rd] = cpu->gpr[instruction->rs] & zero_extend(instruction->immediate);

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_beq(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	int64_t target_offset = (int64_t)(int16_t)instruction->offset * INT64_C(4);
	bool condition = (cpu->gpr[instruction->rs] == cpu->gpr[instruction->rt]);

	// if GPR[rs] = GPR[rt] then branch
	if (condition) {
		cpu->branch_target = cpu->pc + UINT64_C(4) + (uint64_t)target_offset;

		cpu->branch_pending = true;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_beql(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	int64_t target_offset = (int64_t)(int16_t)instruction->offset * INT64_C(4);
	bool condition = (cpu->gpr[instruction->rs] == cpu->gpr[instruction->rt]);

	
	if (condition) {
		cpu->branch_target = cpu->pc + UINT64_C(4) + (uint64_t)target_offset;

		cpu->branch_pending = true;
	}
	else {

	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_daddiu(Mips64CPU* cpu, const Mips64Decoded* instruction) { // Operation: GPR[rd] <- GPR[rs] + GPR[rt];
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	uint64_t source = cpu->gpr[instruction->rs];

	uint64_t immediate = sign_extension(instruction->immediate);

	uint64_t result = source + immediate;

	if (instruction->rt != 0u) {
		cpu->gpr[instruction->rt] = result;
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
		printf("Error or incorrect opcode: 0x%02X\n", (unsigned)instruction->function);
		return MIPS64_STATUS_NOT_IMPLEMENTED;
	}
}

// =================================================================FUNCT==============================
static Mips64Status mips64_execute_add(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (!cpu->gpr[instruction->rs] || cpu->gpr[instruction->rt]) {
		UNPREDICTABLE;
	}

	int64_t l = (int64_t)(int32_t)(uint32_t)cpu->gpr[instruction->rs];
	int64_t r = (int64_t)(int32_t)(uint32_t)cpu->gpr[instruction->rt];

	int64_t temp = l + r;

	if (temp > INT32_MAX || temp < INT32_MIN) {
		return MIPS64_STATUS_INTEGER_OVERFLOW;
	}

	if (instruction->rd != 0) {
		cpu->gpr[instruction->rd] = (uint64_t)(int64_t)(int32_t)temp;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_addu(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}


	int64_t l = (int64_t)(int32_t)(uint32_t)cpu->gpr[instruction->rs];
	int64_t r = (int64_t)(int32_t)(uint32_t)cpu->gpr[instruction->rt];

	int64_t temp = l + r;

	if (instruction->rd != 0) {
		cpu->gpr[instruction->rd] = (uint64_t)(int64_t)(int32_t)temp;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_and(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	cpu->gpr[instruction->rd] = cpu->gpr[instruction->rs] & cpu->gpr[instruction->rd];
	
	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_daddu(Mips64CPU* cpu, const Mips64Decoded* instruction) { // Operation: GPR[rt]  GPR[rs] + sign_extend(immediate)
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}
	
	uint64_t result;

	result = cpu->gpr[instruction->rs] + cpu->gpr[instruction->rt];

	if (instruction->rd != 0) {
		cpu->gpr[instruction->rd] = result;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
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
