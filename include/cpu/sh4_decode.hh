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

#define Rn1()          cpu->get_register(nnnn)
#define Rn2(val)     cpu->set_register(nnnn, val)

#define Rm1()          cpu->get_register(mmmm)
#define Rm2(val)     cpu->set_register(mmmm, val)

#define GET_MACRO(_0, _1, NAME, ...) NAME
#define Rn(...) GET_MACRO(_0 __VA_OPT__(,) __VA_ARGS__,  Rn2, Rn1)(__VA_ARGS__)
#define Rm(...) GET_MACRO(_0 __VA_OPT__(,) __VA_ARGS__,  Rm2, Rm1)(__VA_ARGS__)

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
