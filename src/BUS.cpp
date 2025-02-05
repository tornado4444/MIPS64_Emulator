#include "BUS.hpp"

// CONSTRUCTOR
BUS::BUS() {
	// TODO to the over this
	// Initialize for register
	//for (int i = 0; i < 32; i++) {
	//	registers[i] = 0;
	//}

	//for (auto& i : memory) i = 0x00; // ????

	//MIPS64.connectBus(this); // for the connect the bus
}

// DESTRUCTOR
BUS::~BUS() {

}

// for the write
void BUS::write(uint64_t addr, uint64_t data) {
	if (addr < memory.size())
		*reinterpret_cast<uint64_t*>(&memory[addr]); // memory initialize how source
	else
		std::cerr << "ERROR!";
}

// for the read
uint64_t BUS::read(uint64_t addr, bool bReadOnly) {
	if (addr < memory.size())
		return *reinterpret_cast<uint64_t*>(&memory[addr]);
	else
		std::cerr << "ERROR!";
		return 0x00; // An all-zero octet
}

/*
 ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄       ▄▄▄▄▄▄▄▄▄▄▄  ▄         ▄  ▄▄▄▄▄▄▄▄▄▄▄       ▄▄       ▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌     ▐░░▌     ▐░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌      ▀▀▀▀█░█▀▀▀▀ ▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀▀▀      ▐░▌░▌   ▐░▐░▌ ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀
▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌               ▐░▌▐░▌ ▐░▌▐░▌     ▐░▌     ▐░▌       ▐░▌▐░▌
▐░█▄▄▄▄▄▄▄▄▄ ▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄█░▌          ▐░▌     ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄      ▐░▌ ▐░▐░▌ ▐░▌     ▐░▌     ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄
▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌          ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌  ▐░▌  ▐░▌     ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌▐░█▀▀▀▀█░█▀▀           ▐░▌     ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀      ▐░▌   ▀   ▐░▌     ▐░▌     ▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀█░▌
▐░▌          ▐░▌       ▐░▌▐░▌     ▐░▌            ▐░▌     ▐░▌       ▐░▌▐░▌               ▐░▌       ▐░▌     ▐░▌     ▐░▌                    ▐░▌
▐░▌          ▐░█▄▄▄▄▄▄▄█░▌▐░▌      ▐░▌           ▐░▌     ▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄▄▄      ▐░▌       ▐░▌ ▄▄▄▄█░█▄▄▄▄ ▐░▌           ▄▄▄▄▄▄▄▄▄█░▌
▐░▌          ▐░░░░░░░░░░░▌▐░▌       ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░░░░░░░░░░░▌     ▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░▌          ▐░░░░░░░░░░░▌
 ▀            ▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀            ▀       ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀       ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀            ▀▀▀▀▀▀▀▀▀▀▀

*/
// R - TYPE(instruction)
void BUS::execute_instruction_of_R(uint32_t instruction) {
	uint8_t opcode = (instruction >> 26) & 0x3F; // extract opcode
	uint8_t rs = (instruction >> 21) & 0x1F;	 // extract the rs
	uint8_t rt = (instruction >> 16) & 0x1F;     // extract the rt
	uint8_t rd = (instruction >> 11) & 0x1F;     // extract the rd
	uint8_t shift = (instruction >> 6) & 0x1F;   // extract the shift
	uint8_t funct = instruction & 0x3F;          // extract the funct

	switch (funct) {
	case 0x2D: // DADDU (for the 64 - bit adding)
		registers[rd] = registers[rs] + registers[rt];
		break; // the break this 
	case 0x2F: // DSUBU (for the 64 - bit subtraction)
		registers[rd] = registers[rs] - registers[rt];
		break; // the break this 
	default:
		std::cerr << "This function can be not R - FORMAT: 0x" << std::hex << (int)funct <<std::endl;
		break; // the break this
	}

}

// I - FORMAT
void BUS::execute_instruction_of_I(uint32_t instruction) {
	uint8_t opcode = (instruction >> 26) & 0x3F; // extract opcode
	uint8_t rs = (instruction >> 21) & 0x1F;	 // extract the rs
	uint8_t rt = (instruction >> 16) & 0x1F;     // extract the rt
	uint8_t IMM = instruction >> 0xFFFF;         // extract immediate

	switch (opcode) {
	case 0x23: // LD (64 - bits download)
		registers[rt] = read(registers[rs] + IMM);
		break;
	case 0x2B: // SD (64 - bits saving)
		registers[rt] = read(registers[rs] + IMM);
		break;
	default:
		std::cerr << "This function can be not I - FORMAT: 0x" << std::hex << (int)opcode << std::endl;
		break;
	}
}

// J - FORMAT
void BUS::execute_instruction_of_J(uint32_t instruction) {
	uint8_t opcode = (instruction >> 26) & 0x3F;       // extract opcode
	uint32_t pseudoAdress = instruction >> 0x03FFFFFF; // extract pseudoAdress

	switch (opcode) {
	case 0x02: // J
		pc = (pc & 0xF0000000) | (pseudoAdress >> 2);
		break;
	case 0x03: // JAL
		registers[31] = pc + 8; // SAVING THE ADDRESS RETURN
		pc = (pc & 0xF0000000) | (pseudoAdress >> 2);
		break;
	default:
		std::cerr << "This function can be not J - FORMAT: 0x" << std::hex << (int)opcode << std::endl;
		break;
	}
}
