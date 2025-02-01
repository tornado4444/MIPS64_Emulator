#pragma once
#include <iostream>
#include <cstdint>
#include <array>
#include <iomanip>

#include "olc_64.hpp"

// FOR THE CLASS 
class BUS
{
public:
	BUS();
	~BUS();


	/*friend BUS operator>>(uint32_t value, BUS& pseudoAdress) {
		pseudoAdress.pc = value;
		return pseudoAdress;
	}*/

public: // for the initialization
	olc_64 MIPS64; // announcement MIPS64

	uint64_t registers[32] = {0};              // 32 general purpose registers (64-bit)
	uint64_t pc = 0;					       // program counter
	std::array <uint8_t, 1024 * 1024> memory ; // for the memory on 1 MB
public:
	// INFORMATION FOR THE MIPS INSTRUCTION
	void write(uint64_t pAddr, uint64_t data);             // for the write(64 - bites)
	uint64_t read(uint64_t pAddr, bool bReadOnly = false); // for the read information(64 - bites)
	void execute_instruction_of_R(uint32_t instruction);   // for R - instruction
	void execute_instruction_of_I(uint32_t instruction);   // for I - instruction
	void execute_instruction_of_J(uint32_t instruction);   // for J - instruction
};
