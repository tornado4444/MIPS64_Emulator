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
	MIPS64_OPCODE_J			= 0x02,
	MIPS64_OPCODE_JAL		= 0x03,
	MIPS64_OPCODE_BEQ		= 0x04,
	MIPS64_OPCODE_BNE		= 0x05,
	MIPS64_OPCODE_BLEZ		= 0x06,
	MIPS64_OPCODE_BGTZ		= 0x07,
	MIPS64_OPCODE_ADDI      = 0x08, // +
	MIPS64_OPCODE_ADDIU     = 0x09, // +
	MIPS64_OPCODE_SLTI		= 0xA,
	MIPS64_OPCODE_SLTU		= 0xB,
	MIPS64_OPCODE_ORI		= 0xD,
	MIPS64_OPCODE_LUI		= 0xF,
	MIPS64_OPCODE_LDL		= 0x1A,
	MIPS64_OPCODE_LDR		= 0x1B,
	MIPS64_OPCODE_JALX		= 0x1D,
	MIPS64_OPCODE_BEQL      = 0x14,  // WARNING: Removed for Release 6
	MIPS64_OPCODE_BNEL		= 0x15,
	MIPS64_OPCODE_BLEZL		= 0x16, // WARNING: Removed for Release 6
	MIPS64_OPCODE_BGTZL		= 0x17, // WARNING: Removed for Release 6
	MIPS64_OPCODE_DADDI		= 0x18,
	MIPS64_OPCODE_DADDIU    = 0x19,
	MIPS64_OPCODE_LB		= 0x20,
	MIPS64_OPCODE_LH		= 0x21,
	MIPS64_OPCODE_LWL		= 0x22,
	MIPS64_OPCODE_LW		= 0x23,
	MIPS64_OPCODE_LBU		= 0x24,
	MIPS64_OPCODE_LHU		= 0x25,
	MIPS64_OPCODE_LWR		= 0x26,
	MIPS64_OPCODE_LWU		= 0x27,
	MIPS64_OPCODE_SH		= 0x29,
	MIPS64_OPCODE_SDL		= 0x2C,
	MIPS64_OPCODE_SDR		= 0x2D,
	MIPS64_OPCODE_LL		= 0x30,
	MIPS64_OPCODE_LWC1		= 0x31,
	MIPS64_OPCODE_LWC2		= 0x32,
	MIPS64_OPCODE_PREF		= 0x33,
	MIPS64_OPCODE_LLD		= 0x34,
	MIPS64_OPCODE_LDC1		= 0x35,
	MIPS64_OPCODE_LDC2		= 0x36,
	MIPS64_OPCODE_LD		= 0x37,
	MIPS64_OPCODE_SC		= 0x38,
	MIPS64_OPCODE_SCD		= 0x3C,
	MIPS64_OPCODE_SDC1		= 0x3D,
	MIPS64_OPCODE_SDC2		= 0x3E,
	MIPS64_OPCODE_SD		= 0x3F,
} Mips64Opcode;

typedef enum Mips64Function {
	MIPS64_FUNCT_SLL		= 0x00,
	MIPS64_FUNCT_SRL		= 0x00,
	MIPS64_FUNCT_MOVF		= 0x01,
	MIPS64_FUNCT_MOVCI		= 0x01,
	MIPS64_FUNCT_SRL		= 0x02,
	MIPS64_FUNCT_SRA		= 0x03,
	MIPS64_FUNCT_SLLV		= 0x04,
	MIPS64_FUNCT_SRAV		= 0x07,
	MIPS64_FUNCT_JR			= 0x08,
	MIPS64_FUNCT_JR_HB		= 0x08,
	MIPS64_FUNCT_JALR		= 0x09,
	MIPS64_FUNCT_JALR_HB	= 0x09,
	MIPS64_FUNCT_MOVZ		= 0xA,
	MIPS64_FUNCT_ROTRV		= 0xA,
	MIPS64_FUNCT_MOVN		= 0xB,
	MIPS64_FUNCT_ANDI		= 0xC,
	MIPS64_FUNCT_BREAK		= 0xD,
	MIPS64_FUNCT_MFHI		= 0x10,
	MIPS64_FUNCT_MTHI		= 0x11,
	MIPS64_FUNCT_MFLO		= 0x12,
	MIPS64_FUNCT_MTLO		= 0x13,
	MIPS64_FUNCT_DSLLV		= 0x14,
	MIPS64_FUNCT_DROTRV		= 0x16,	// Only for MIPS64 Release 2
	MIPS64_FUNCT_DSRLV		= 0x16,	
	MIPS64_FUNCT_DSRAV		= 0x17,
	MIPS64_FUNCT_MULT		= 0x18,
	MIPS64_FUNCT_MULTU		= 0x19,
	MIPS64_FUNCT_DIV		= 0x1A,
	MIPS64_FUNCT_DIVU		= 0x1B,
	MIPS64_FUNCT_DMULT		= 0X1C,
	MIPS64_FUNCT_DMULTU		= 0X1D,
	MIPS64_FUNCT_DDIV		= 0x1E,
	MIPS64_FUNCT_DDIVU		= 0x1F,
	MIPS64_FUNCT_ADD		= 0x20, // +
	MIPS64_FUNCT_ADDU		= 0x21, // +
	MIPS64_FUNCT_AND		= 0x24, // +
	MIPS64_FUNCT_OR			= 0x25, // +
	MIPS64_FUNCT_NOR		= 0x27, // +
	MIPS64_FUNCT_SB			= 0x28, // +
	MIPS64_FUNCT_SLT		= 0x2A,
	MIPS64_FUNCT_SLTU		= 0x2B,
	MIPS64_FUNCT_DADD	    = 0x2C,
	MIPS64_FUNCT_DADDU		= 0x2D,
	MIPS64_FUNCT_DSUB		= 0x30,
	MIPS64_FUNCT_DSUBU		= 0x31,
	MIPS64_FUNCT_DSLL		= 0x38,
	MIPS64_FUNCT_DROTR		= 0x3A,	// Only for MIPS64 Release 2
	MIPS64_FUNCT_DSRL		= 0x3A,	
	MIPS64_FUNCT_DSRA		= 0x3B,
	MIPS64_FUNCT_DSLL32		= 0x3C,
	MIPS64_FUNCT_DROTR32	= 0x3E,	
	MIPS64_FUNCT_DSRL32		= 0x3E,	// Only for MIPS64 Release 2
	MIPS64_FUNCT_DSRA32		= 0x3F,	
} Mips64Function;

// RELEASE DECODE INSTRUCTION
void mips64_decode_instruction(uint32_t raw, Mips64Decoded* out_instruction);


