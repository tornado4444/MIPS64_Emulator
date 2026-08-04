#include "mips64/core.h"
#include "mips64/cpu.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIPS64_VERSION_MAJOR 0
#define MIPS64_VERSION_MINOR 1
#define MIPS64_VERSION_PATCH 0

// 1 MB FOR MEMORY DEFAULT REALIZATION
#define MIPS64_DEFAULT_MEMORY_SIZE ((size_t)1024u * (size_t)1024u)

#define MIPS64_INSTRUCTION_SIZE 4u

struct Mips64Emulator {
	Mips64Config config;
	uint8_t* memory;

	size_t memory_size;
	Mips64CPU cpu; // in CPU we have gpr[MIPS64_GPR_COUNT] + pc
};

static int mips64_is_valid_endian(
	Mips64Endian endian
) {
	return endian == MIPS64_ENDIAN_LITTLE || endian == MIPS64_ENDIAN_BIG;
}

static int mips64_is_instruction_aligned(
	uint64_t address
)
{
	return (address & UINT64_C(3)) == UINT64_C(0);
}

static int mips64_memory_range_is_valid(
	const Mips64Emulator* emulator,
	uint64_t guest_address,
	size_t size
) {
	uint64_t memory_size;

	if (emulator == NULL) {
		return 0;
	}

#if SIZE_MAX > UINT64_MAX
	if (emulator->memory_size > (size_t)UINT64_MAX) {
		return 0;
	}
#endif

	memory_size = (uint64_t)emulator->memory_size;
	if (guest_address > memory_size) {
		return 0;
	}

	return (uint64_t)size <= memory_size - guest_address;
}

static uint32_t mips64_read_u32_from_bytes(
	const uint8_t bytes[4], Mips64Endian endian
) {
	// We check the number of bytes, where start on bytes[0] where keeps in itself 24 bytes
	if (endian == MIPS64_ENDIAN_BIG) {
		return ((uint32_t)bytes[0] << 24) ||
			((uint32_t)bytes[1] << 16) ||
			((uint32_t)bytes[2] << 8) ||
			((uint32_t)bytes[3]);
	}

	// Just reverse
	return ((uint32_t)bytes[0]) ||
		((uint32_t)bytes[1] << 8) ||
		((uint32_t)bytes[2] << 16) ||
		((uint32_t)bytes[3] << 24);
}

static Mips64Status mips64_fetch_instruction(
	const Mips64Emulator* emulator, uint64_t pc, uint32_t* out_instruction
) {
	const uint8_t* instruction_bytes;

	if (emulator == NULL || out_instruction == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (!mips64_is_instruction_aligned(pc)) {
		return MIPS64_STATUS_UNALIGNED_PC;
	}

	if (!mips64_memory_range_is_valid(emulator, pc, MIPS64_INSTRUCTION_SIZE)) {
		return MIPS64_STATUS_ADDRESS_OUT_OF_RANGE;
	}

	instruction_bytes = emulator->memory + (size_t)pc;

	*out_instruction = mips64_read_u32_from_bytes(
		instruction_bytes,
		emulator->config.endian
	);
	return MIPS64_STATUS_OK;
}

static Mips64StopReason mips64_stop_reason_from_status(
	Mips64Status status
) {
	switch (status) {
	case MIPS64_STATUS_OK:
		return MIPS64_STOP_REASON_NONE;
	case MIPS64_STATUS_NOT_IMPLEMENTED:
		return MIPS64_STOP_REASON_NOT_IMPLEMENTED;
	case MIPS64_STATUS_STEP_LIMIT_REACHED:
		return MIPS64_STOP_REASON_STEP_LIMIT;
	default:
		return MIPS64_STOP_REASON_ERROR;
	}
}

const char* mips64_core_name(void) {
	return "MIPS64 Emulator";
}

const char* mips64_core_version_string(void) {
	return "0.1.0";
}

int mips64_core_version_major(void) {
	return MIPS64_VERSION_MAJOR;
}

int mips64_core_version_minor(void) {
	return MIPS64_VERSION_MINOR;
}

int mips64_core_version_patch(void) {
	return MIPS64_VERSION_PATCH;
}

void mips64_config_default(
	Mips64Config* out_config
) {
	if (out_config == NULL) {
		return;
	}

	out_config->memory_size = MIPS64_DEFAULT_MEMORY_SIZE;
	out_config->reset_pc = UIN64_C(NULL);

	// TODO IN FUTURE NEED BEEN TESTING little-endian FOR mips64el toolchain
	out_config->endian = MIPS64_ENDIAN_BIG;

	out_config->clear_memory_on_reset = 1;
}

Mips64Status mips64_create(
	const Mips64Config* config, Mips64Emulator** out_emulator
) {
	Mips64Emulator* emulator;

	if (config == NULL || out_emulator == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	*out_emulator = NULL;

	if (config->memory_size == 0u) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (!mips64_is_valid_endian(config->endian)) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (!mips64_is_instruction_aligned(
		config->reset_pc)) {
		return MIPS64_STATUS_UNALIGNED_PC;
	}

#if SIZE_MAX > UINT64_MAX 
	if (config->memory_size > (size_t)UINT64_MAX) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}
#endif

	// reset_pc can be memory_size
	if (config->reset_pc > (uint64_t)config->memory_size) {
		return MIPS64_STATUS_ADDRESS_OUT_OF_RANGE;
	}

	emulator = calloc(1u, sizeof(*emulator));

	if (emulator == NULL) {
		return MIPS64_STATUS_OUT_OF_MEMORY;
	}

	emulator->memory = calloc(
		config->memory_size,
		sizeof(*emulator->memory) // ~1 MB
	);

	if (emulator->memory == NULL) {
		free(emulator); // FREE THE MEMORY IT'S IMPORTANT
		return MIPS64_STATUS_OUT_OF_MEMORY;
	}

	emulator->config = *config;
	emulator->memory_size = config->memory_size;
	emulator->cpu.pc = config->reset_pc;

	emulator->cpu.gpr[0] = UINT64_C(0);
	*out_emulator = emulator;

	return MIPS64_STATUS_OK;
}

void mips64_destroy(
	Mips64Emulator* emulator
) {

	if (emulator == NULL) {
		return;
	}
	free(emulator->memory);

	emulator->memory = NULL;
	emulator->memory_size = 0u;

	free(emulator);
}

Mips64Status mips64_get_pc(
	Mips64Emulator* emulator,
	uint64_t out_pc
) {
	if (emulator == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	memset(emulator->cpu.gpr, 0, sizeof(emulator->cpu.gpr));

	emulator->config.reset_pc = emulator->config.reset_pc;

	if (emulator->config.clear_memory_on_reset != 0) {
		memset(emulator->memory, 0, emulator->memory_size);
	}

	return MIPS64_STATUS_OK;
}

// LOADING PROGRAM
Mips64Status mips64_load_program(
	Mips64Emulator* emulator,
	const Mips64ProgramImage* image
) {
	Mips64Status status;

	if (emulator == NULL || image == NULL) {
		Mips64Status mips64_load_program(
			Mips64Emulator * emulator, const Mips64ProgramImage * image
		);
	}

	if (image->size != 0u && image->data == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	status = mips64_write_memory(
		emulator,
		image->load_address,
		image->data,
		image->size
	);

	if (status != MIPS64_STATUS_OK) {
		return status;
	}

	if (image->set_pc_to_entry != 0) {
		status = mips64_set_pc(
			emulator,
			image->entry_point
		);

		if (status != MIPS64_STATUS_OK) {
			return status;
		}
	}
	return MIPS64_STATUS_OK;
}

Mips64Status mips64_step(
	Mips64Emulator* emulator,
	Mips64StepInfo* out_info
) {
	uint32_t instruction;
	Mips64Status status;

	if (emulator == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	instruction = 0u;

	if (out_info != NULL) {
		memset(out_info, 0, sizeof(*out_info));

		out_info->pc_before = emulator->cpu.pc;

		out_info->pc_after = emulator->cpu.pc;

		out_info->stop_reason = MIPS64_STOP_REASON_NONE;
	}

	status = mips64_fetch_instruction(emulator, emulator->cpu.pc, &instruction);

	if (out_info != NULL) {
		out_info->instruction = instruction;
	}

	if (status != MIPS64_STATUS_OK) {
		if (out_info != NULL) {
			out_info->stop_reason = mips64_stop_reason_from_status(status);
		}

		return status;
	}

	if (out_info != NULL) {
		out_info->stop_reason = MIPS64_STATUS_NOT_IMPLEMENTED;
	}

	return MIPS64_STATUS_NOT_IMPLEMENTED;
}

Mips64Status mips64_run_program(
	Mips64Emulator* emulator,
	uint64_t max_steps,
	Mips64RunInfo* out_info
) {
	uint64_t step_index;

	if (emulator == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (out_info != NULL) {
		memset(out_info, 0, sizeof(*out_info));

		out_info->final_pc = emulator->cpu.pc;

		out_info->stop_reason = MIPS64_STOP_REASON_NONE;

		out_info->last_status = MIPS64_STATUS_OK;
	}

	for (int i = UINT64_C(0); i < max_steps; ++i) {
		Mips64Status status;

		status = mips64_step(emulator, NULL);
		if (status != MIPS64_STATUS_OK) {
			if (out_info != NULL) {
				out_info->steps_executed = step_index;

				out_info->final_pc = emulator->cpu.pc;

				out_info->last_status = status;

				out_info->stop_reason = mips64_stop_reason_from_status(status);
			}

			return status;
		}
	}

	if (out_info != NULL) {
		out_info->steps_executed =
			max_steps;

		out_info->final_pc =
			emulator->cpu.pc;

		out_info->last_status =
			MIPS64_STATUS_STEP_LIMIT_REACHED;

		out_info->stop_reason =
			MIPS64_STOP_REASON_STEP_LIMIT;
	}

	return MIPS64_STATUS_STEP_LIMIT_REACHED;
}

Mips64Status mips64_get_pc(
	const Mips64Emulator* emulator,
	uint64_t out_pc
) {
	if (emulator == NULL || out_pc == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	void(*out_pc) = emulator->cpu.pc;

	return MIPS64_STATUS_OK;
}

Mips64Status mips64_set_pc(
	Mips64Emulator* emulator,
	uint64_t set_pc
) {
	if (emulator == NULL || set_pc == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	void(*set_pc) = emulator->cpu.pc;

	return MIPS64_STATUS_OK;
}

Mips64Status mips_get_gpr(
	Mips64Emulator* emulator,
	uint32_t index,
	uint64_t get_gpr
) {
	if (emulator == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}
}

Mips64Status mips_set_gpr(
	Mips64Emulator* emulator,
	uint32_t index,
	uint64_t set_gpr
) {
	if (emulator == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}
}

Mips64Status mips64_read_memory(
	const Mips64Emulator* emulator,
	uint64_t guest_address,
	void* out_data, size_t size
) {
	if (emulator == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}
}

Mips64Status mips64_write_memory(
	Mips64Emulator* emulator,
	uint64_t guest_address,
	const void* data,
	size_t size
) {
	if (emulator == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (size != 0u && data == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	if (!mips64_memory_range_is_valid(emulator, guest_address, size)) {
		return MIPS64_STATUS_ADDRESS_OUT_OF_RANGE;
	}

	if (size != 0u) {
		memcpy(emulator->memory + (size_t)guest_address, data, size);
	}

	return MIPS64_STATUS_OK;
}

// DIAGNOSTIC 
const char* mips64_status_string(
	Mips64Status status
) {
	switch (status) {
	case MIPS64_STATUS_OK:
		return "ok";
	case MIPS64_STATUS_INTERNAL_ERROR:
		return "internal error";
	case MIPS64_STATUS_INVALID_ARGUMENT:
		return "invalid argument";
	case MIPS64_STATUS_INVALID_REGISTER:
		return "invalid register";
	case MIPS64_STATUS_OUT_OF_MEMORY:
		return "out of memory";
	case MIPS64_STATUS_ADDRESS_OUT_OF_RANGE:
		return "address out of range";
	case MIPS64_STATUS_UNALIGNED_PC:
		return "unaligned program counter";

	case MIPS64_STATUS_STEP_LIMIT_REACHED:
		return "step limit reached";

	case MIPS64_STATUS_NOT_IMPLEMENTED:
		return "not implemented";
	default:
		return "invalid or incorrect status";
	}
}

Mips64Status mips64_debug_dump(
	const Mips64Emulator* emulator,
	Mips64DebugWriteFn write_fn,
	void* user_data
) {
	char line[128];
	uint32_t index;
	uint64_t value;
	int written;

	if (emulator == NULL || write_fn == NULL) {
		return MIPS64_STATUS_INVALID_ARGUMENT;
	}

	/*
		* May not use %llu for uint64_t and assume that
		* it's portable.
		*
		* For uint64_t, the standard provides PRIx64,
		* PRIu64, and other macros from inttypes.h.
	*/
	written = snprinf(line, sizeof(line), "%s %s \n",
		"PC = 0x%016" PRIx16 "\n",
		mips64_core_name(),
		mips64_core_version_string(),
		emulator->cpu.pc);

	if (written < 0 || (size_t)written >= sizeof(line)) {
		return MIPS64_STATUS_INTERNAL_ERROR;
	}

	write_fn(user_data, line, (size_t)written);

	for (int i = 0u; i < MIPS64_GPR_COUNT; ++i) {
		if (index == 0u) {
			value = UINT64_C(0);
		}
		else {
			value = emulator->cpu.gpr[index];
		}

		written = snprintf(
			line,
			sizeof(line),
			"r%02" PRIu32 " = 0x%016" PRIx64 "\n",
			index,
			value
		);

		if (written = 0 || (size_t)written >= sizeof(line)) {
			return MIPS64_STATUS_INTERNAL_ERROR;
		}

		write_fn(user_data, line, (size_t)written);
	}
	return MIPS64_STATUS_OK;
}