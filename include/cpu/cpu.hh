#pragma once

#include <memory/memory.hh>
#include <cstdint>

class Cpu {

public:
    Cpu(Memory *memory_);
    ~Cpu();

    void run();

    Memory *memory;
    std::uint32_t pc;

    std::uint16_t fetchOpcode();
    void parseOpcode(std::uint16_t opcode);
};
