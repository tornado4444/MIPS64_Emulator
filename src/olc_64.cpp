#include "olc_64.hpp"

class Bus; // INITIALIZE FROM THE BUS.HPP

// CONSTRUCTOR
olc_64::olc_64() {
    for (int i = 0; i < 32; i++) {  // for the bites etc.
        registers[i] = 0;
    }

    pc = 0; // Program  Counter
    hi = 0; // Register HI
    lo = 0; // Register LO

    using a = olc_64;

    lookup = {
        {}
        
    };
}

/*
 ███▄ ▄███▓ ██▓ ██▓███    ██████     ██▓ ███▄    █   ██████ ▄▄▄█████▓ ██▀███   █    ██  ▄████▄  ▄▄▄█████▓ ██▓ ▒█████   ███▄    █
▓██▒▀█▀ ██▒▓██▒▓██░  ██▒▒██    ▒    ▓██▒ ██ ▀█   █ ▒██    ▒ ▓  ██▒ ▓▒▓██ ▒ ██▒ ██  ▓██▒▒██▀ ▀█  ▓  ██▒ ▓▒▓██▒▒██▒  ██▒ ██ ▀█   █
▓██    ▓██░▒██▒▓██░ ██▓▒░ ▓██▄      ▒██▒▓██  ▀█ ██▒░ ▓██▄   ▒ ▓██░ ▒░▓██ ░▄█ ▒▓██  ▒██░▒▓█    ▄ ▒ ▓██░ ▒░▒██▒▒██░  ██▒▓██  ▀█ ██▒
▒██    ▒██ ░██░▒██▄█▓▒ ▒  ▒   ██▒   ░██░▓██▒  ▐▌██▒  ▒   ██▒░ ▓██▓ ░ ▒██▀▀█▄  ▓▓█  ░██░▒▓▓▄ ▄██▒░ ▓██▓ ░ ░██░▒██   ██░▓██▒  ▐▌██▒
▒██▒   ░██▒░██░▒██▒ ░  ░▒██████▒▒   ░██░▒██░   ▓██░▒██████▒▒  ▒██▒ ░ ░██▓ ▒██▒▒▒█████▓ ▒ ▓███▀ ░  ▒██▒ ░ ░██░░ ████▓▒░▒██░   ▓██░
░ ▒░   ░  ░░▓  ▒▓▒░ ░  ░▒ ▒▓▒ ▒ ░   ░▓  ░ ▒░   ▒ ▒ ▒ ▒▓▒ ▒ ░  ▒ ░░   ░ ▒▓ ░▒▓░░▒▓▒ ▒ ▒ ░ ░▒ ▒  ░  ▒ ░░   ░▓  ░ ▒░▒░▒░ ░ ▒░   ▒ ▒
░  ░      ░ ▒ ░░▒ ░     ░ ░▒  ░ ░    ▒ ░░ ░░   ░ ▒░░ ░▒  ░ ░    ░      ░▒ ░ ▒░░░▒░ ░ ░   ░  ▒       ░     ▒ ░  ░ ▒ ▒░ ░ ░░   ░ ▒░
░      ░    ▒ ░░░       ░  ░  ░      ▒ ░   ░   ░ ░ ░  ░  ░    ░        ░░   ░  ░░░ ░ ░ ░          ░       ▒ ░░ ░ ░ ▒     ░   ░ ░
       ░    ░                 ░      ░           ░       ░              ░        ░     ░ ░                ░      ░ ░           ░
    ░


    1. General 3-operand format:
    - op    dest, src1, src2     dest <- src1 op src -----------------> dest, src1, src2 are registers
    2. Addition:
    - add   a, b, c              a <- b + c;         ----------------->
    - addi(unsigned) a, b, 12    a <- b + 12;        -----------------> The "i" in addi if for immediate
    3. Substraction:
    - sub   a, b, c              a <- b-c;
    4. Complex:
    f = (g + h) - (i + j);
    - add t0, g, h      t0 <- g + h;
    - add t1, i, j      t1 <- i + j;
    - sub f, t0, t1     f  <- t0 - t1;  ------------> Complex oprations generate many instructions with temporary values
*/

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

// THE DATA TRASFER
void olc_64::dataTransfer(uint32_t instruction) {
    uint8_t opcode = (instruction >> 26) & 0x3F;   // Select opcode
    uint8_t base = (instruction >> 21) & 0x1F;   // The base register(rs)
    uint8_t rt = (instruction >> 16) & 0x1F;   // Destination register (rt)
    uint16_t offset = instruction & 0xFFFF;         // OFFSET

    // Calculate the address in memory : base + offset
    uint64_t address = registers[base] + static_cast<int64_t>(offset);

    switch (opcode) {
    case 0x23:  // lw(Load Word)
        registers[rt] = read(address); // To read the 32 bites for the memory
        break;
    case 0x21:  // lh (Load Halfword)
        registers[rt] = static_cast<int64_t>(static_cast<int16_t>(read(address) & 0xFFFF)); // To read the 16 bites and symbolically expand
        break;
    case 0x20:  // lb(Load Byte)
        registers[rt] = static_cast<int64_t>(static_cast<int8_t>(read(address) & 0xFF));    // Read 8 bits and sign expand
        break;
    case 0x37: // Read 8 bits and sign expand
        registers[rt] = read(address);  // To read 64 bites for the memory
        break;

        // Store instructions
    case 0x2B:  // sw (Store Word)
        write(address, registers[rt] & 0xFFFFFFFF); // Write 32 bits to memory
        break;
    case 0x29:                                      // sh(Store Halfword)
        write(address, registers[rt] & 0xFFFF);     // Write 16 бит to memory
        break;
    case 0x28:                                      // sb (Store Byte)
        write(address, registers[rt] & 0xFF);       // Write 8 bits to memory
        break;
    case 0x3F:                                      // sd (Store Doubleword) - approximate opcode
        write(address, registers[rt]);              // Write 64 bits в память
        break;
    default:
        std::cerr << "NOTHING HERE 0x : " << std::hex << instruction << std::endl; // for the message error
        break;
    }
}

void olc_64::handleSyscall() {
    switch (registers[2]) { // Syscall number in $v0 ($2)
    case 1: // Print integer
        std::cout << registers[4] << std::endl;
        break;
    case 10: // Exit
        std::exit(0);
        break;
    default:
        std::cerr << "Unknown syscall: " << registers[2] << std::endl;
        break;
    }
}

// SYSTEM WORK
void olc_64::systemWork(uint32_t instructions) {
    uint8_t opcode = (instructions >> 26) & 0x3F;           // OPCODE
    uint8_t funct = instructions & 0x3F;                   // FUNCT
    uint8_t rd = (instructions >> 11) & 0x1F;              // RD
    uint8_t rt = (instructions >> 16) & 0x1F;              // RT
    uint8_t rs = (instructions >> 21) & 0x1F;              // RS

    switch (funct) {
    case 0x0C: // SYSCALL
        handleSyscall();
        break;
    case 0x0D: // BREAK
        std::cerr << "BREAK instruction encountered at PC = 0x" << std::hex << pc << std::endl;
        // В реальном эмуляторе здесь можно перейти в режим отладки
        break;
    case 0x18: // ERET (Exception Return)
        pc = coprocessor0[31]; // Предположим, что EPC хранится в регистре 31 сопроцессора 0
        break;
    case 0x00: // MFC0 (Move from Coprocessor 0)
        if (rd < 32) {
            registers[rt] = coprocessor0[rd];
        }
        else {
            throw std::runtime_error("Invalid coprocessor 0 register");
        }
        break;
    case 0x04: // MTC0 (Move to Coprocessor 0)
        if (rd < 32) {
            coprocessor0[rd] = registers[rt];
        }
        else {
            throw std::runtime_error("Invalid coprocessor 0 register");
        }
        break;
    default:
        std::cerr << "Unknown system instruction: 0x" << std::hex << instructions << std::endl;
        break;
    }
}



// R - TYPE INSTRUCTIONS
void olc_64::handleInstruction_of_R(uint32_t instructions) {
    uint8_t rs = (instructions >> 21) & 0x1F;
    uint8_t rt = (instructions >> 16) & 0x1F;
    uint8_t rd = (instructions >> 11) & 0x1F;
    uint8_t shamt = (instructions >> 6) & 0x1F;
    uint8_t funct = instructions & 0x3F;        

    // FOR THE JR
    uint64_t jumpAddress = registers[rs];       // move to address from rs register

    switch (funct) {
    case 0x20: // ADD
        registers[rd] = registers[rs] + registers[rt];
        break;
    case 0x21: // ADDU
        registers[rd] = static_cast<uint64_t>(registers[rs]) + static_cast<uint64_t>(registers[rt]);
        break;
    case 0x24: // AND
        registers[rd] = registers[rs] & registers[rt];
        break;
    case 0x1A: // DIV
        if (registers[rt] != 0) {
            lo = static_cast<int64_t>(registers[rs]) / static_cast<int64_t>(registers[rt]);
            hi = static_cast<int64_t>(registers[rs]) % static_cast<int64_t>(registers[rt]);
        }
        break;
    case 0x1B: // DIVU
        if (registers[rt] != 0) {
            lo = static_cast<uint64_t>(registers[rs]) / static_cast<uint64_t>(registers[rt]);
            hi = static_cast<uint64_t>(registers[rs]) % static_cast<uint64_t>(registers[rt]);
        }
        break;
    case 0x09:  // JALR
        registers[rd] = pc + 4; // SAVE THE ADDRESS TO RETURN 
        pc = registers[rs];     // Go to address from rs
        break;
    case 0x22: // SUB
        registers[rd] = registers[rs] - registers[rt];
        break;
    case 0x23: // SUBU
        registers[rd] = static_cast<uint64_t>(registers[rs]) - static_cast<uint64_t>(registers[rt]);
        break;
    case 0x18: // MULT
    {
        int64_t result = static_cast<int64_t>(registers[rs]) * static_cast<int64_t>(registers[rt]);
        hi = (result >> 32) & 0xFFFFFFFF;
        lo = result & 0xFFFFFFFF;
        break;
    }
    case 0x19: // MULTU
    {
        uint64_t result = static_cast<uint64_t>(registers[rs]) * static_cast<uint64_t>(registers[rt]);
        hi = (result >> 32) & 0xFFFFFFFF;
        lo = result & 0xFFFFFFFF;
        break;
    }
    case 0x25: // OR
        registers[rd] = registers[rs] | registers[rt];
        break;
    case 0x26: // XOR
        registers[rd] = registers[rs] ^ registers[rt];
        break;
    case 0x27: // NOR
        registers[rd] = ~(registers[rs] | registers[rt]);
        break;
    case 0x2A: // SLT
        registers[rd] = (static_cast<int64_t>(registers[rs]) < static_cast<int64_t>(registers[rt])) ? 1 : 0;
        break;
    case 0x2B: // SLTU
        registers[rd] = (static_cast<uint64_t>(registers[rs]) < static_cast<uint64_t>(registers[rt])) ? 1 : 0;
        break;
    case 0x00: // SLL
        registers[rd] = registers[rt] << shamt;
        break;
    case 0x02: // SRL
        registers[rd] = static_cast<uint64_t>(registers[rt]) >> shamt;
        break;
    case 0x03: // SRA
        registers[rd] = static_cast<int64_t>(registers[rt]) >> shamt;
        break;
    case 0x08: // JR(Jump to Address in Register)
        pc = registers[rt];
        break;
    default:
        std::cerr << "Unknown R-type instruction: " << std::hex << instructions << std::endl;
        break;
    }
}

// I - TYPE INSTRUCTIONS
void olc_64::handleInstruction_of_I(uint32_t instruction) {
    uint8_t opcode = (instruction >> 26) & 0x3F;
    uint8_t rs = (instruction >> 21) & 0x1F;
    uint8_t rt = (instruction >> 16) & 0x1F;
    int16_t offset = instruction & 0xFFFF;
    int64_t signed_offset = static_cast<int64_t>(offset);

    switch (opcode) {
    case 0x04: // BEQ
        if (registers[rs] == registers[rt]) {
            pc += signed_offset;
        }
        break;
    case 0x05: // BNE
        if (registers[rs] != registers[rt]) {
            pc += signed_offset;
        }
        break;
    case 0x06: // BLEZ
        if (registers[rs] <= 0) {
            pc += signed_offset;
        }
        break;
    case 0x07: // BGTZ
        if (registers[rs] > 0) {
            pc += signed_offset;
        }
        break;
    case 0x08: // ADDI
        registers[rt] = registers[rs] + offset;
        break;
    case 0x09: // ADDIU
        registers[rt] = static_cast<uint64_t>(registers[rs]) + static_cast<uint64_t>(offset);
        break;
    case 0x0C: // ANDI
        registers[rt] = registers[rs] & static_cast<uint64_t>(offset);
        break;
    case 0x0D: // ORI
        registers[rt] = registers[rs] | static_cast<uint64_t>(offset);
        break;
    case 0x0E: // XORI
        registers[rt] = registers[rs] ^ static_cast<uint64_t>(offset);
        break;
    case 0x0F: // LUI
        registers[rt] = static_cast<uint64_t>(offset) << 16;
        break;
    default:
        std::cerr << "Unknown I-type instruction: " << std::hex << instruction << std::endl;
        break;
    }
}

// FOR THE J - INSTRUCTIONS
void olc_64::handleInstruction_of_J(uint32_t instruction) {
    uint8_t opcode = (instruction >> 26) & 0x3F; // 26 - bites for opcode
    uint32_t address = instruction & 0x03FFFFFF;  // Allocating a 26-bit address

    // MIPS64 for J - INSTRUCTIONS
    uint64_t jumpAddress = (pc & 0xFFFFFFFFF0000000) | (address << 2);

    // Unconditional Jump
    switch (opcode) {
    case 0x02:                  // JUMP(J)
        pc = jumpAddress;       // assign the jump adress
        break;
    case 0x03:                  // JUMP AND LINK(JAL)
        registers[31] = pc;     // THE SAVE THIS ADDRESS RETURN
        pc = jumpAddress;       // assign the jump adress
        break;
    default:
        std::cerr <<"NOTHING HERE 0x : " << std::hex << instruction << std::endl;
        break;
    }
}

// FOR THE OLC EXECUTE INSTRUCTION
void olc_64::executeInstruction(uint32_t instruction) {
	uint8_t opcode = (instruction >> 26) & 0x3F; // we declare opcode, when mask for extracting specific bits from an instruction

    if (opcode == 0x00) {
        handleInstruction_of_R(instruction); // R - instructions
    }
    else if (opcode == 0x02 || opcode == 0x03) {
        handleInstruction_of_J(instruction); // J(JUMP) or JUMP AND LINK
    }
    else {
        handleInstruction_of_I(instruction); // I - instructions
    }
}


