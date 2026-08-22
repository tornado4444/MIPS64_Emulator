#pragma once

#include "mips64/decode.h"

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
