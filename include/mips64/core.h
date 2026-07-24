#pragma once

#include <stddef.h>
#include <stdint.h>

// ADDING extern "C" 
#ifdef __cplusplus
extern "C" {
#endif

#if defined(MIPS64_CORE_STATIC)
	#define MIPS64_API
#elif defined(_WIN32) // FOR API WINDOWS!!!
	#if defined(MIPS64_CORE_BUILD_SHARED)
		#define MIPS64_API __declspec(dllexport) // ONLY FOR MODULE FOR EXPORT
	#else	
		#define MIPS64_API __declspec(dllimport) // ONLY FOR MODULE FOR IMPORT
	#endif
#elif defined(__GNUC__) || defined(__clang__) // FOR LINUX YEAHHH!
	#define MIPS64_API __attribute__((visibility("default")))
#else
	#define MIPS64_API 
#endif

#define MIPS64_GPR_COUNT 32u

typedef enum Mips64Status {
	MIPS64_STATUS_OK = 0,
	MIPS64_STATUS_INVALID_ARGUMENT,
	MIPS64_STATUS_OUT_OF_MEMORY,

	MIPS64_STATUS_ADDRESS_OUT_OF_RANGE,
	MIPS64_STATUS_INVALID_REGISTER,
	MIPS64_STATUS_UNALIGNED_PC,
	
	MIPS64_STATUS_STEP_LIMIT_REACHED,
	MIPS64_STATUS_NOT_IMPLEMENTED,

	MIPS64_STATUS_INTERNAL_ERROR
} Mips64Status;

typedef enum Mips64Endian {
	MIPS64_ENDIAN_LITTLE = 0,
	MIPS64_ENDIAN_BIG = 1
} Mips64Endian;

typedef enum Mips64StopReason {
	MIPS64_STOP_REASON_NONE = 0,

	MIPS64_STOP_REASON_NOT_IMPLEMENTED,
	MIPS64_STOP_REASON_STEP_LIMIT,
	MIPS64_STOP_REASON_ERROR
} Mips64StopReason;

typedef struct Mips64Config {
	size_t memory_size;
	uint64_t reset_pc;

	Mips64Endian endian;
	/* TODO
	- CPU reset;
	- machine reset;
	- memory clear;
	*/
	int clear_memory_on_reset;
} Mips64Config;

typedef struct Mips64ProgramImage {
	const void* data;
	size_t size;

	uint64_t load_address;
	uint64_t entry_point;
	int set_pc_to_entry;
} Mips64ProgramImage;

typedef struct Mips64StepInfo {
	uint64_t pc_before;
	uint64_t pc_after; 

	uint32_t instruction;

	Mips64StopReason stop_reason;
} Mips64StepInfo;

typedef struct Mips64RunInfo {
	uint64_t steps_executed;
	uint64_t step_final;
	uint64_t final_pc;

	Mips64StopReason stop_reason;
	Mips64Status last_status;
} Mips64RunInfo;

typedef struct Mips64Emulator Mips64Emulator;

typedef void(*Mips64DebugWriteFn) (
	void* user_data, const char* data, size_t size 
);

MIPS64_API const char* mips64_core_name(void);
MIPS64_API const char* mips64_core_version_string(void);

MIPS64_API int mips64_core_version_major(void);
MIPS64_API int mips64_core_version_minor(void);
MIPS64_API int mips64_core_version_patch(void);

MIPS64_API void mips64_config_default(
	Mips64Config* out_config
);

MIPS64_API Mips64Status mips64_create(
	const Mips64Config* config, Mips64Emulator** out_emulator
);

MIPS64_API void mips64_destroy(
	Mips64Emulator* emulator
);

MIPS64_API Mips64Status mips64_load_program(
	Mips64Emulator* emulator, const Mips64ProgramImage* image
);

MIPS64_API Mips64Status mips64_step(
	Mips64Emulator* emulator,
	Mips64StepInfo* out_info
);

MIPS64_API Mips64Status mips64_run_program(
	Mips64Emulator* emulator,
	uint64_t max_steps,
	Mips64RunInfo* out_info
);

MIPS64_API Mips64Status mips64_get_pc(
	const Mips64Emulator* emulator, uint64_t out_pc
);

MIPS64_API Mips64Status mips64_set_pc(
	Mips64Emulator* emulator, uint64_t set_pc 
);

MIPS64_API Mips64Status mips_get_gpr(
	Mips64Emulator* emulator, uint32_t index, uint64_t get_gpr
);

MIPS64_API Mips64Status mips_set_gpr(
	Mips64Emulator* emulator, uint32_t index, uint64_t set_gpr
);

MIPS64_API Mips64Status mips64_read_memory(
	const Mips64Emulator* emulator, uint64_t guest_address, 
	void* out_data, size_t size
);

MIPS64_API Mips64Status mips64_write_memory(
	Mips64Emulator* emulator, uint64_t guest_address,
	const void* out_data, size_t size
);

// DIAGNOSTIC 
MIPS64_API const char* mips64_status_string(
	Mips64Status status
);

MIPS64_API Mips64Status mips64_debug_dump(
	const Mips64Emulator* emulator, Mips64DebugWriteFn write_fn, void* user_data
);

#ifdef __cplusplus
}
#endif