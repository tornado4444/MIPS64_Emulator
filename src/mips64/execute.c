#include <stdio.h>
#include <immintrin.h>
#include <intrin.h>

#include "mips64/execute.h"


static uint64_t sign_extension(uint16_t value)
{
	return (uint64_t)(int64_t)(int16_t)value;
}

static uint64_t zero_extend(uint16_t zero_extend_value) {
	return (uint64_t)zero_extend_value;
}

static bool NotWordValue(uint64_t value) {
	return value != (uint64_t)(int64_t)(uint32_t)(int32_t)value;
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
		cpu->branch_mode = MIPS64_BRANCH_LIKELY;

		cpu->pc += 4;
	}
	else {
		cpu->pc += 8;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_bgtz(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	/*
		Operation:
		I: target_offset ← sign_extend(offset || 0^2)
			condition ← GPR[rs] > 0^GPRLEN
		0^GPRLEN - for MIPS64 it's 64 zero bits, since in our case it will read the full 64-bits
		I+1: if condition then
			PC ← PC + target_offset
		endif
	*/

	int64_t target_offset = (int64_t)(int16_t)instruction->offset * INT64_C(4);
	bool condition = cpu->gpr[instruction->rs] > INT64_C(0);  

	if (condition) {
		cpu->branch_target = cpu->pc + UINT64_C(4) + (uint64_t)target_offset;

		cpu->branch_pending = true;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_bgtz(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	int64_t target_offset = (int64_t)(int16_t)instruction->offset * INT64_C(4);
	bool condition = (cpu->gpr[instruction->rs] > INT64_C(0));

	if (condition) {
		cpu->branch_target = cpu->pc + UINT64_C(4) + (uint64_t)target_offset;

		cpu->branch_pending = true;
		cpu->branch_mode = MIPS64_BRANCH_LIKELY;

		cpu->pc += 4;
	}
	else {
		cpu->pc += 8;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_blez(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	int64_t target_offset = (int64_t)(int16_t)instruction->offset * INT64_C(4);
	bool condition = cpu->gpr[instruction->rs] <= INT64_C(0);

	if (condition) {
		cpu->branch_target = cpu->pc + UINT64_C(4) + (uint64_t)target_offset;

		cpu->branch_pending = true;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_blezl(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	int64_t target_offset = (int64_t)(int16_t)instruction->offset * INT64_C(4);
	bool condition = cpu->gpr[instruction->rs] <= INT64_C(0);

	if (condition) {
		cpu->branch_target = cpu->pc + UINT64_C(4) + (uint64_t)target_offset;

		cpu->branch_pending = true;
		cpu->branch_mode = MIPS64_BRANCH_LIKELY;

		cpu->pc += 4;
	}
	else {
		cpu->pc += 8;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_bne(Mips64CPU* cpu, const Mips64Decoded* instruction)  {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}


	int64_t target_offset = (int64_t)(int16_t)instruction->offset * INT64_C(4);
	bool condition = (cpu->gpr[instruction->rs] != cpu->gpr[instruction->rt]);

	if (condition) {
		cpu->branch_target = cpu->pc + UINT64_C(4) + (uint64_t)target_offset;

		cpu->branch_pending = true;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_bnel(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}


	int64_t target_offset = (int64_t)(int16_t)instruction->offset * INT64_C(4);
	bool condition = (cpu->gpr[instruction->rs] != cpu->gpr[instruction->rt]);

	if (condition) {
		cpu->branch_target = cpu->pc + UINT64_C(4) + (uint64_t)target_offset;

		cpu->branch_pending = true;
		cpu->branch_mode = MIPS64_BRANCH_LIKELY;

		cpu += 4;
	}
	else {
		cpu += 8;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

// TODO CACHE
// static Mips64Status mips64_execute_daddi(Mips64CPU* cpu, const Mips64Decoded* instruction) {}

static Mips64Status mips64_execute_daddi(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	int64_t lhs = (int64_t)cpu->gpr[instruction->rs];
	int64_t imm = (int64_t)(int16_t)instruction->immediate;

	/*
		TODO CHECK ON FUTURE THE TEST FOR CORRECT! OPERATION FROM DOCUMENTATION:
			temp ← (GPR[rs]63||GPR[rs]) + sign_extend(immediate)
			if (temp64 ≠ temp63) then
				SignalException(IntegerOverflow)
			else
				GPR[rt] ← temp63..0
			endif
	*/
	if ((imm > 0 && lhs > INT64_MAX - imm) || (imm < 0 && lhs < INT64_MIN - imm)) {
		return MIPS64_STATUS_INTEGER_OVERFLOW;
	}

	int64_t temp = lhs + imm;

	if (instruction->rt != 0) {
		cpu->gpr[instruction->rt] = (uint64_t)temp;
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

	uint64_t imm = sign_extension(instruction->immediate);

	uint64_t temp = source + imm;

	if (instruction->rt != 0u) {
		cpu->gpr[instruction->rt] = temp;
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

static Mips64Status mips64_execute_dadd(Mips64CPU* cpu, const Mips64Decoded* instruction) { // Operation: GPR[rt]  GPR[rs] + sign_extend(immediate)
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	int64_t rhs = (int64_t)cpu->gpr[instruction->rs];
	int64_t rts = (int64_t)cpu->gpr[instruction->rt];

	if ((rts > 0 && rhs > INT64_MAX - rts) || (rts < 0 && rhs < INT64_MIN - rts)) {
		return MIPS64_STATUS_INTEGER_OVERFLOW;
	}

	if (instruction->rd != 0) {
		cpu->gpr[instruction->rd] = cpu->gpr[instruction->rs] + cpu->gpr[instruction->rt];
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_daddu(Mips64CPU* cpu, const Mips64Decoded* instruction) { // Operation: GPR[rt]  GPR[rs] + sign_extend(immediate)
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}
	
	uint64_t temp = cpu->gpr[instruction->rs] + cpu->gpr[instruction->rt];

	if (instruction->rd != 0) {
		cpu->gpr[instruction->rd] = temp;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}


static Mips64Status mips64_execute_ddiv(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	(int64_t)cpu->lo = cpu->gpr[instruction->rs] / cpu->gpr[instruction->rt];
	(int64_t)cpu->hi = cpu->gpr[instruction->rs] % cpu->gpr[instruction->rt];

	if (cpu->gpr[instruction->rt] != 0) {
		UNPREDICTABLE;
	}

	cpu->gpr[0] = INT64_C(0);
	cpu->pc += INT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_ddivu(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}
	uint64_t inst_rs = cpu->gpr[instruction->rs];
	uint64_t inst_rt = cpu->gpr[instruction->rt];


	if (cpu->gpr[instruction->rt] == UINT64_C(0)) {
		UNPREDICTABLE;
	}

	uint64_t q = inst_rs / inst_rt;
	uint64_t r = inst_rs % inst_rt;

	cpu->lo = q;
	cpu->hi = r;

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += INT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_div(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	int32_t q = (int32_t)cpu->gpr[instruction->rs] / (int32_t)cpu->gpr[instruction->rt];
	cpu->lo = (uint64_t)(int64_t)q;
	int32_t r = (int32_t)cpu->gpr[instruction->rs] % (int32_t)cpu->gpr[instruction->rt];
	cpu->hi = (uint64_t)(int64_t)r;

	if (NotWordValue(cpu->gpr[instruction->rs]) || NotWordValue(cpu->gpr[instruction->rt])) {
		UNPREDICTABLE;
	}

	cpu->gpr[0] = INT64_C(0);
	cpu->pc += INT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_divu(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	uint32_t q = (uint32_t)cpu->gpr[instruction->rs] / (uint32_t)cpu->gpr[instruction->rt];
	uint32_t r = (uint32_t)cpu->gpr[instruction->rs] % (uint32_t)cpu->gpr[instruction->rt];
	cpu->lo = (uint64_t)(int64_t)q;
	cpu->hi = (uint64_t)(int64_t)r;

	if (NotWordValue(cpu->gpr[instruction->rs]) || NotWordValue(cpu->gpr[instruction->rt])) {
		UNPREDICTABLE;
	}

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_dmult(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	int64_t p = (int64_t)cpu->gpr[instruction->rs];
	int64_t d = (int64_t)cpu->gpr[instruction->rt];

	int64_t prod = p * d;

	cpu->lo = prod;
	cpu->hi = _umul128(p, d, & prod);

	cpu->gpr[0] = INT64_C(0);
	cpu->pc += INT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_dmultu(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	uint64_t p = cpu->gpr[instruction->rs];
	uint64_t d = cpu->gpr[instruction->rt];

	uint64_t prod = p * d;

	cpu->lo = prod;
	cpu->hi = _umul128(p, d, &prod);

	cpu->gpr[0] = UINT64_C(0);
	cpu->pc += UINT64_C(4);

	return MIPS64_STATUS_OK;
}

static Mips64Status mips64_execute_break(Mips64CPU* cpu, const Mips64Decoded* instruction) {
	if (cpu == NULL || instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	return MIPS64_STATUS_BREAKPOINT;
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
