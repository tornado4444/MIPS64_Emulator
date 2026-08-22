#pragma once

#include <stdint.h>

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
	- MIPS64 Release 2 and Release 6 + other Releases;
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
		IN FUTURE NEED BEEN TO TRY THIS OUT, OTHERWISE THERE WILL TOO MANY INSTRUCTIONS!!! NEED FIX THAT MANNNNNN! DAMN, DON'T HAVE MUCH TIME
	*/
	MIPS64_OPCODE_SPECIAL,
	MIPS64_OPCODE_DADDIU,
	MIPS64_OPCODE_DADDU
} Mips64Opcode;

// RELEASE DECODE INSTRUCTION
static void decode_instruction(uint32_t raw, Mips64Decoded* out_instruction);
static uint64_t sign_extension(uint16_t value);

