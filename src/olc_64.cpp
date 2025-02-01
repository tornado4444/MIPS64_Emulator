#include "olc_64.hpp"

class Bus; // INITIALIZE FROM THE BUS.HPP

// CONSTRUCTOR
olc_64::olc_64() {

}

// DESTRUCTOR
olc_64::~olc_64() {

}

// FOR THE READ
uint64_t olc_64::read(uint64_t a) {
	return bus->read(a, false);
}

// FOR THE WRITE
void olc_64::write(uint64_t a, uint64_t d) {
	bus->write(a, d);
}


// FOR THE OLC HANDLE INSTRUCTION
void olc_64::handleInstruction(uint32_t instruction) {
	uint8_t rs = (instruction >> 21) & 0x1F; // for the rs
	uint8_t rt = (instruction >> 16) & 0x1F; // for the rt
	int16_t offset = instruction & 0xFFFF;   // BIAS

	int64_t signed_offset = static_cast<int64_t>(offset); // Signed offset extension to 64 bits

	if ((instruction >> 26) == 0x04) {         // BEQ
		if (registers[rs] == registers[rt]) { // for the logic 
			pc += signed_offset << 2;		 // shift left by 2 bits (multiply by 4) taking into account sign extension
		}
	}
	else if ((instruction >> 26) == 0x05) { // BNE
		if (registers[rs] != registers[rt]) {
			pc += signed_offset << 2; // Transition taking into account offset
		}
	} 
}

// FOR THE HANDLE ARITHMETIC
void olc_64::handleArithmetic(uint32_t instruction) {
	uint8_t rs = (instruction >> 21) & 0x1F;	// Source 1
	uint8_t rt = (instruction >> 16) & 0x1F;	// Source 2
	uint8_t rd = (instruction >> 11) & 0x1F;	// Destination
	uint8_t shamt = (instruction >> 6) & 0x1F;  // Shift (not used in most operations)
	uint8_t funct = instruction & 0x3F;			// Function (defines a specific operation)

	switch (funct) { // for the funct
	case 0x18:       // MULTIPLY
		registers[rs] = registers[rd] = registers[rs] * registers[rt];
		break;
	case 0x20:		 // ADD
		registers[rs] = registers[rd] = registers[rs] + registers[rt];
		break;
	case 0x22:		 // SUB
		registers[rd] = registers[rs] - registers[rt];
		break;
	case 0x24:		 // AND
		registers[rd] = registers[rs] & registers[rt];
		break;
	case 0x25:		 // OR
		registers[rd] = registers[rs] | registers[rt];
		break;
	case 0x2A:		 // SLT (Set Less Than)
		registers[rd] = (registers[rs] < registers[rt]) ? 1 : 0;
		break;
	case 0x1A:		 // DIVIDE
		registers[rd] = registers[rs] / registers[rt];
		break;
	default:
		std::cerr << "Unknow the arithmetic: " << std::hex << instruction << std::endl;
		break;
	}
}

// FOR THE OLC EXECUTE INSTRUCTION
void olc_64::executeInstruction(uint32_t instruction) {
	uint8_t opcode = (instruction >> 26) & 0x3F; // 

	switch (opcode) {
	case 0x00: // Opcode extraction(first 6 bits)
		handleArithmetic(instruction);
		break;
	case 0x04: // BEQ
	case 0x05:
		handleInstruction(instruction); // Handle branches
		break;
	default:
		std::cerr << "This is unknow instruction: " << std::hex << instruction << std::endl;
		break;
	}
}