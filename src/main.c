#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "mips64/core.h"
#include "mips64/cpu.h"
#include "mips64/memory.h"
#include "mips64/decode.h"
#include "mips64/execute.h"

static int start_program(void) {
	/*
	 * VERY IMPORTANTANT!!!
	 * Memory has 64 MB
	 * There is no need to create it locally on the stack...bad idea
	 * */
	static Memory memory;

	Mips64CPU cpu;

	// Little Endian
	if (mips64_memory_init(&memory, MIPS64_ENDIAN_LITTLE) != MIPS64_MEMORY_OK) {
		fprintf(stderr, "Failed initialize memory\n");
		return 0;
	}

	/*
	 * START FROM NULL ADDRESS
	 */

	mips64_cpu_init(&cpu, UINT64_C(0x00000000));

	/*
	 * TEST PROGRAM
	 * For that example:
	 *
	 * DADDIU $1, $0, 10,
	 * DADDIU $2, $0, 20,
	 * DADDU  $3, $0, $2;
	 *
	 * ALSO IMPORTANT WHAT IT'S EXAMPLE SPECIFICALLY FOR THIS INSTRUCTIONS, OF COURSE, IF WE ADDED ADDITIONAL INSTRUCTIONS THERE WOULD BE A DIFFERENT EXAMPLE FOR THE TEST.
	 * BUT FOR THE TEST I'M SHOWING SIMPLE TEST FOR THESE INSTRUCTIONS(INCLUDING OPCODES + FUNCTION), BUT NEXT TIME WHEN BEEN SERIOUSLY USE INSTRUCTIONS, I WILL DISPLAY THEM THROUGH THE GUI,
	 * BE IT RELEASE 2 OR RELEASE 6, BUT THIS WILL BE ON FUTURE, JUST FOR NOW I WANT TO CONVEY WHAT IS CURRENTLY BEING EXECUTED.
	 *
	 * Print:
	 *
	 * GPR[1] = 10 -> GPR[2] = 20 -> GPR[3] = 30;
	 *
	 * Every instruction MIPS32/MIPS64 base ISA takes 32 bits. adresses:
	 *
	 * instruction 0 -> 0x0000
	 * instruction 1 -> 0x0004
	 * instruction 2 -> 0x0008
	 * */
	const uint32_t program[] = {
		/*
			* DADDIU $1, $0, 10 -> 0x011001
			*
			* opcode = DADDIU
			* rs     = 0
			* rt     = 1
			* imm    = 10
			*/

		((uint32_t)MIPS64_OPCODE_DADDIU << 26) | (0u << 21) | (1u << 16) | 10u,
		/*
		 * DADDU $2, $0, 20
		 * */
		((uint32_t)MIPS64_OPCODE_DADDIU << 26) | (0u << 21) | (2u << 16) | 20u,
		/*
		 * DADDU $3, $1, $2
		 *
		 * opcode = SPECIAL -> 0x000000
		 * rs     = 1
		 * rt     = 2
		 * rd     = 3
		 * shamt  = 0
		 * funct  = DADDU
		 *
		 * MIPS64_FUNCT_DADDU must be function code, not primary code
		 * */
		((uint32_t)MIPS64_OPCODE_SPECIAL << 26) | (1u << 21) | (2u << 16) | (3u << 11) | (0u << 6) | MIPS64_FUNCT_DADDU
	};
	const size_t instruction_count = sizeof(program) / sizeof(program[0]);

	// LOAD PROGRAM INTO EMULATED MEMORY
	for (size_t i = 0; i < instruction_count; ++i) {
		const uint64_t address = (uint64_t)i * UINT64_C(4);

		Mips64MemoryStatus status = mips64_memory_write32(&memory, address, program[i]);

		if (status != MIPS64_MEMORY_OK) {
			fprintf(stderr, "Failed to write instruction %zu at 0x%016" PRIx64 "\n", i, address);
			return 1;
		}
	}

	// FETCH -> DECODE -> EXECUTE

	for (size_t instruction_step = 0; instruction_step < instruction_count; ++instruction_step) {
		uint64_t pc = 0;

		if (mips64_cpu_get_pc(&cpu, &pc) != MIPS64_STATUS_OK) {
			fprintf(stderr, "Failed to read PC\n");
			return 0;
		}

		/*
		* FETCH
		*
		* PC HAS ADDRESS INSTRUCTION
		*
		* 32 bits instruction
		*/
		uint32_t raw_instruction = 0;

		Mips64MemoryStatus memory_status = mips64_memory_read32(&memory, pc, &raw_instruction);

		if (memory_status != MIPS64_MEMORY_OK) {
			fprintf(stderr, "Instruction fetch failed at PC=0x%016" PRIx64 "\n", pc);
			return 0;
		}

		printf("STEP %zu\n""  PC  = 0x%016" PRIx64 "\n""  RAW = 0x%08" PRIx32 "\n", instruction_step, pc, raw_instruction);

		// DECODE
		Mips64Decoded decode;

		mips64_decode_instruction(raw_instruction, &decode);

		printf(
			"  opcode = 0x%02" PRIx8 "\n"
			"  rs     = %" PRIu8 "\n"
			"  rt     = %" PRIu8 "\n"
			"  rd     = %" PRIu8 "\n"
			"  shamt  = %" PRIu8 "\n"
			"  funct  = 0x%02" PRIx8 "\n"
			"  imm    = 0x%04" PRIx16 "\n",
			decode.opcode,
			decode.rs,
			decode.rt,
			decode.rd,
			decode.shift_amount,
			decode.function,
			decode.immediate
		);

		// EXECUTE
		Mips64Status ex_status = mips64_cpu_execute(&cpu, &decode);

		if (ex_status != MIPS64_STATUS_OK) {
			fprintf(stderr, "Execution failed at PC=0x%016" PRIx64 "\n", pc);
			return 1;
		}

		// WHILE HAVEN'T branches/jumps/exceptions, JUST WRITE PC + 4
		if (mips64_cpu_set_pc(&cpu, pc + UINT64_C(4)) != MIPS64_STATUS_OK) {
			fprintf(stderr, "Failed to update PC\n");
			return 1;
		}

		printf("\n");
	}

	// VERIFY RESULTS
	uint64_t r1 = 0;
	uint64_t r2 = 0;
	uint64_t r3 = 0;

	mips64_cpu_mips_get_gpr(&cpu, 1, &r1);
	mips64_cpu_mips_get_gpr(&cpu, 2, &r2);
	mips64_cpu_mips_get_gpr(&cpu, 3, &r3);

	printf("Final registers:\n");
	printf("  $1 = %" PRIu64 "\n", r1);
	printf("  $2 = %" PRIu64 "\n", r2);
	printf("  $3 = %" PRIu64 "\n", r3);

	if (r1 != UINT64_C(10)) {
		fprintf(stderr, "TEST FAILED: $1 != 10\n");
		return 0;
	}

	if (r2 != UINT64_C(20)) {
		fprintf(stderr, "TEST FAILED: $2 != 20\n");
		return 0;
	}

	if (r3 != UINT64_C(30)) {
		fprintf(stderr, "TEST FAILED: $3 != 30\n");
		return 0;
	}

	printf("\nTEST PASSED\n");
	return 0;
}

int main(void) {
	return start_program();
}