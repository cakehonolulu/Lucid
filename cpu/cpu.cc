#include <cpu/cpu.hh>
#include <lucid.hh>
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
    uint16_t opcode = (memory->read(pc + 1) << 8) | memory->read(pc);
    return opcode;
}

void Cpu::parseOpcode(uint16_t opcode) {

    uint8_t function = (opcode >> 12) & 0xF;

    switch (function) {
        case 0b1110:
            std::cout << BOLDWHITE << "mov #" << +((std::int8_t)(opcode & 0xFF)) << ", r" << ((opcode & 0x0F00) >> 8) << "\n";
            //exit(1);
            break;

        default:
            std::cerr << BOLDRED << "Unimplemented opcode: 0x" << format("{:04X}", opcode) << " (Function bits: 0b" << format("{:04b}", function) << ")" << RESET << "\n";
            exit(1);
            break;
    }
}
