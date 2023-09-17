#include <cpu/cpu.hh>
#include <iostream>

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

Cpu::Cpu(Memory *memory_)
{
    pc = 0x00000000;
    memory = memory_;
}

Cpu::~Cpu() {
}

void Cpu::run() {
    while (true) {
        uint16_t opcode = fetchOpcode();

        parseOpcode(opcode);

        pc += 2;
    }
}

uint16_t Cpu::fetchOpcode() {
    uint16_t opcode = (memory->read(pc) << 8) | memory->read(pc + 1);
    return opcode;
}

void Cpu::parseOpcode(uint16_t opcode) {

    uint8_t instr = (opcode >> 10) & 0x3F;

    switch (instr) {
        
        default:
            std::cerr << "Unimplemented opcode: 0x" << format("{:02X}", instr) << "\n";
            exit(1);
            break;
    }
}
