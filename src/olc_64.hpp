/*
 ▄▄▄▄▄▄▄▄▄▄▄  ▄            ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄         ▄
▐░░░░░░░░░░░▌▐░▌          ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌       ▐░▌
▐░█▀▀▀▀▀▀▀█░▌▐░▌          ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌
▐░▌       ▐░▌▐░▌          ▐░▌          ▐░▌          ▐░▌       ▐░▌
▐░▌       ▐░▌▐░▌          ▐░▌          ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌
▐░▌       ▐░▌▐░▌          ▐░▌          ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░▌       ▐░▌▐░▌          ▐░▌          ▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀▀▀▀▀▀█░▌
▐░▌       ▐░▌▐░▌          ▐░▌          ▐░▌       ▐░▌          ▐░▌
▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌          ▐░▌
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌          ▐░▌
 ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀            ▀

*/
#pragma once
#include "BUS.hpp"

class olc_64
{
public:
	// CONSTRUCTOR
	olc_64();
	// DESTRUCTOR
	~olc_64();

	// for the bus
	void connectBus(BUS* n) { bus = n; }
private:
	// REGISTERS ON MIPS
	uint64_t registers[32];				// for the 32 registers(R0 - R31)
	uint64_t pc;						// Program Counter
	uint64_t hi;						// Register hi(for the multiplication/division)
	uint64_t lo;						// Register lo(for the multiplication/division)
	
	uint32_t coprocessor0[32] = { 0 };  // Coprocessor registers 0

private:
	void dataTransfer(uint32_t instruction); // for the data transwer Load/Store instructions
	void handleSyscall();	 // for the syscall
	void systemWork(uint32_t instructions);	 // instructions for working with the system: syscall, break, erat, mfc0, mtc0, etc.
private:
	BUS* bus = nullptr;						 // for the bus
	uint64_t read(uint64_t a);               // for the read
	void write(uint64_t a, uint64_t d);		 // for the write

	// ADDRESSING MODES
	/*
	 -----------------------------------------------------------------------------------------------
	|		Mode		|		Example	   |		Meaning			|			When used			|
	|-------------------|------------------|------------------------|-------------------------------|
	|  Register direct	|  add R1, R2, R3  |    R1 = R2 + R3		|	when value is in register	|
	|  Immediate		|  addi R1, R2, R3 |    R1 = R2 + 3			|	for constants				|
	|  Reg ind w/ displ |  lw R1, 100(R2)  |	R1 = Mem[R2 + 100]  |	for array elements			|
	 -----------------------------------------------------------------------------------------------

	 - register indirect: displacement is 0
	 - Absolute adressing: R0 base register
	 - Mode bit specifies Big Endian / Little Endian
	 - Data bit specified in opcode:
		lb, lh, lw, ld, l.s, l.d
	 - Memory accesses must be alligned. 
	 */

	void handleInstruction_of_R(uint32_t instruction);  // instruction(of R)
	void handleInstruction_of_I(uint32_t instruction);  // instruction(of I)
	void handleInstruction_of_J(uint32_t instruction);  // instruction(of J)
	void executeInstruction(uint32_t instruction);		// Execute more instruction
};

