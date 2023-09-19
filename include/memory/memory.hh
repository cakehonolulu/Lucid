#pragma once

#include <cpu/sh4_cpu.hh>
#include <string>
#include <vector>
#include <cstdint>

class Memory {

public:

    Memory();
    ~Memory();

    std::uint8_t* bios;  // Pointer for BIOS (2MB)
    std::uint8_t* flash; // Pointer for Flash (256KB)
    std::uint8_t* main_memory; // Pointer for main memory (16MB)
    std::uint8_t* vram;  // Pointer for VRAM (8MB)

    void load_bios(const std::string& bios_path);
    void load_flash(const std::string& flash_path);
    std::uint8_t read(uint32_t address, Sh4_Cpu *cpu);
};