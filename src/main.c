#include <inttypes.h>
#include <stdio.h>

#include "mips64/core.h"
#include "mips64/cpu.h"
#include "mips64/memory.h"
#include "mips64/decode.h"

void start_program(void) {
	/*
	 * VERY IMPORTANTANT!!!
	 * Memory has 64 MB
	 * There is no need to create it locally on the stack...bad idea
	 * */
	static Memory memory;

	Mips64CPU cpu;

	// Little Endian
	if(mips64_memory_init(&memory, MIPS64_LITTLE_ENDIAN)) {
		fprintf(stderr, "Failed initialize memory\n");
		return 1;
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

		((uint32_t) MIPS64_OPCODE_DADDIU << 26) | (0u << 21) | (1u << 16) | 10u,
		/*
		 * DADDU $2, $0, 20
		 * */
		((uint32_t) MIPS64_OPCODE_DADDIU << 26) | (0u << 21) | (2u << 16) | 20u,
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
		((uint32_t) MIPS64_FUNCT_DADDU << 26) | (1u << 21) | (2u << 16) | (3u << 11) | (0u << 6) | MIPS64_FUNCT_DADDU
	};
}

int main(int argc, char** argv) {
	start_program(void);	
	return 0;
}
