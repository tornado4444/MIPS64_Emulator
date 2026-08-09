#pragma once

#include "mips64/core.h"

// RELEASE THE CPU MIPS64	
typedef struct Mips64CPU {
	uint64_t gpr[MIPS64_GPR_COUNT];

	uint64_t pc; // Program Count register
	/* TODO NEED BEEN RELEASE ON FUTURE FOR
	* uint64_t hi; // Multiply and Divide register higher result
	* uint64_t lo; // Multiply and Divide register lower result
	*/
} Mips64CPU;

// DECODING INSTRUCTIONS
/* TODO
	This is temporary, since it is just for testing,
	there are few instructions and in fact it will take a long time and be difficult to do at first,
	then I will change it. In general, this will need to be transferred to decode.c when it is available
	- Ten instructions;
	- Multiple formats;
	- Operation code tables;
	- Disassembler;
	- Decoder tests separate from the CPU;
	- Different ISA versions;
	- MIPS64 Release 2 and Release 6;
	- microMIPS;
	Separate representation of the decoded instruction.
	In addition, we must not forget that coprocessors are not implemented, so there is simply no point in adding the possibility now.
*/
typedef struct Mips64Decoded {
	uint32_t raw;

	uint8_t opcode;
	uint8_t rs;
	uint8_t rt;
	uint8_t rd;
	uint8_t shift_amount;
	uint8_t function;

	uint16_t immediate;
	uint32_t target;
} Mips64Decoded;

typedef enum Mips64Opcode {
	/* TODO
		* It's a base opcodes instructions, in future need add more:
		* - ADD 
		* - ADDU
		* - ORI
		* - ANDI
		* - XORI
		* - LUI
		* - DSUBU
		* - SLT
		* - SLTU
	*/
	MIPS64_OPCODE_SPECIAL,
	MIPS64_OPCODE_DADDIU,
	MIPS64_OPCODE_DADDU
} Mips64Opcode;

void mips64_cpu_init(Mips64CPU* cpu, uint64_t reset_pc);
void mips64_cpu_reset(Mips64CPU* cpu, uint64_t reset_pc);
Mips64Status mips64_cpu_mips_get_gpr(const Mips64CPU* cpu, uint32_t index, uint64_t* out_value);
Mips64Status mips64_cpu_set_gpr(Mips64CPU* cpu, uint32_t index, uint64_t value);
Mips64Status mips64_cpu_get_pc(const Mips64CPU* cpu, uint64_t* out_pc);
Mips64Status mips64_cpu_set_pc(Mips64CPU* cpu, uint64_t pc);

// RELEASE DECODE INSTRUCTION
static void decode_instruction(uint32_t raw, Mips64Decoded* out_instruction);
static uint64_t sign_extension(uint16_t value);

static Mips64Status mips64_cpu_execute(Mips64CPU* cpu, uint32_t raw_instruction);
static Mips64Status mips64_execute_special(Mips64CPU* cpu, const Mips64Decoded* instruction);
static Mips64Status mips64_execute_nop(Mips64CPU* cpu, const Mips64Decoded* instruction); //  0x000000
static Mips64Status mips64_execute_daddiu(Mips64CPU* cpu, const Mips64Decoded* instruction); // Operation: GPR[rd] <- GPR[rs] + GPR[rt];
static Mips64Status mips64_execute_daddu(Mips64CPU* cpu, const Mips64Decoded* instruction); // Operation: GPR[rt]  GPR[rs] + sign_extend(immediate) 

