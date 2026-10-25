#pragma once

#include <stdint.h>

// DECODING INSTRUCTIONS
/* TODO
	This is temporary, since it is just for testing,
	there are few instructions and in fact it will take a long time and be difficult to do at first,
	then I will change it. In general, this will need to be transferred to decode.c when it is available
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
	uint16_t offset;
	uint32_t target;
} Mips64Decoded;

typedef enum Mips64Opcode {
	MIPS64_OPCODE_SPECIAL   = 0x00,
	MIPS64_OPCODE_BEQ		= 0x04,
	MIPS64_OPCODE_BNE		= 0x05,
	MIPS64_OPCODE_BLEZ		= 0x06,
	MIPS64_OPCODE_BGTZ		= 0x07,
	MIPS64_OPCODE_ADDI      = 0x08,
	MIPS64_OPCODE_ADDIU     = 0x09,
	MIPS64_OPCODE_DADDIU    = 0x19,
	MIPS64_OPCODE_BEQL		= 0x20, // WARNING: Removed for Release 6
	MIPS64_OPCODE_BLEZL		= 0x22, // WARNING: Removed for Release 6
	MIPS64_OPCODE_BGTZL		= 0x23, // WARNING: Removed for Release 6
	MIPS64_OPCODE_BC		= 0x32, // WARNING: BC for Release 6
	MIPS64_OPCODE_BALC		= 0x3A, // WARNING: BALC for Release 6
} Mips64Opcode;

typedef enum Mips64Function {
	MIPS64_FUNCT_ADD		= 0x20,
	MIPS64_FUNCT_ADDU		= 0x21,
	MIPS64_FUNCT_DADDU      = 0x2D,
} Mips64Function;

// RELEASE DECODE INSTRUCTION
void mips64_decode_instruction(uint32_t raw, Mips64Decoded* out_instruction);


