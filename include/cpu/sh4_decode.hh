#pragma once

#include <memory/memory.hh>
#include <cpu/sh4_cpu.hh>
#include <lucid.hh>
#include <iostream>

class Sh4_Decode {

private:

public:

    Memory *memory;
    Sh4_Cpu *cpu;

    Sh4_Decode(Sh4_Cpu *cpu_, Memory *memory_);

    void run();
    uint16_t fetch_opcode();
    void parse_opcode(uint16_t opcode);
};
