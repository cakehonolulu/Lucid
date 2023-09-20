#pragma once

#include <memory/memory.hh>
#include <cpu/sh4_cpu.hh>
#include <lucid.hh>
#include <iostream>

#define GET_REG(idx)        (cpu->get_register(idx))
#define SET_REG(idx, val)   (cpu->set_register(idx, val))

#define GET_PC()            (cpu->get_pc())
#define SET_PC(val)         (cpu->set_pc(val))

#define GET_DELAY_PC()      (cpu->get_delay_pc())
#define SET_DELAY_PC(val)   (cpu->set_delay_pc(val))

#define GET_TBIT()          (cpu->get_tbit())
#define SET_TBIT(val)       (cpu->set_tbit(val))

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
