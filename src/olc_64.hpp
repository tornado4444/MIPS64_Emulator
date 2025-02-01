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
	enum MIPS64 {
		C = (1 << 0), // CARRY BIT
		Z = (1 << 1), // ZERO
		I = (1 << 2), // DISABLE INTERRUPTS
		D = (1 << 3), // DECIMAL MODE
		B = (1 << 4), // BREAK
		U = (1 << 5), // UNUSED
		V = (1 << 6), // OVERFLOW
		N = (1 << 7)  // NEGATIVE
	};

public:
	// CONSTRUCTOR
	olc_64();
	// DESTRUCTOR
	~olc_64();

	// for the bus
	void connectBus(BUS* n) { bus = n; }
private:
	BUS* bus = nullptr;                   // for the bus
	uint64_t read(uint64_t a);           // for the read
	void write(uint64_t a, uint64_t d); // for the write

	// REGISTERS ON MIPS
	uint64_t registers[32]; // for the 32 registers(R0 - R31)
	uint64_t pc;			// Program Counter
	uint64_t hi;			// Register hi(for the multiplication/division)
	uint64_t lo;			// Register lo(for the multiplication/division)

	void executeInstruction(uint32_t instruction); // Execute one instruction
	void handleInstruction(uint32_t instruction);  // Processing conditional branches
	void handleArithmetic(uint32_t instruction);   // Arithmetic processing
};

