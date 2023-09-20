#pragma once

#include <cpu/sh4_cpu.hh>
#include <lucid.hh>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

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
    
    template <typename T>
    T read(uint32_t address, Sh4_Cpu *cpu) {
        
        // Get MD Bit (Bit #30) from the Status Register
        bool md_bit = cpu->get_md_bit();

        bool p = (address >> 31) & 1;
        bool alt = (address >> 30) & 1;
        bool nc = (address >> 29) & 1;

        T *from;

//#ifdef MEMORY_DEBUG
        std::cout << "memory_read: Reading from 0x" << format("{:08X}", address) << ", P: " << +(p) << " , ALT: " 
        << +(alt) << " , NC: " << +(nc) << std::endl;

        if (address >= 0 && address <= 0x7FFFFFFF)
        {
            std::cout << "             U0/P0 read (";
        }
        else
        if (address >= 0x80000000 && address <= 0x9FFFFFFF)
        {
            std::cout << "             P1 read (";
        }
        else
        if (address >= 0xA0000000 && address <= 0xBFFFFFFF)
        {
            std::cout << "             P2 read (";
        }
        else
        if (address >= 0xC0000000 && address <= 0xDFFFFFFF)
        {
            std::cout << "             P3 read (";
        }
        else
        if (address >= 0xE0000000 && address <= 0xFFFFFFFF)
        {
            std::cout << "             P4 read (";
        }
//#endif

        // Calculate the physical address
        std::uint32_t p_addr = (address & 0x1FFFFFFF);

//#ifdef MEMORY_DEBUG
        std::cout << "Physical: 0x" << format("{:08X}", p_addr) << ")" << std::endl;
//#endif

        // $00000000 - $001FFFFF | Boot ROM (2MB)
        if (p_addr >= 0x00000000 && p_addr <= 0x001FFFFF)
        {
            from = reinterpret_cast<T*>(&bios[p_addr]);
        }
        else if (p_addr >= 0x00200000 && p_addr <= 0x0023FFFF)
        {
            from = reinterpret_cast<T*>(&flash[p_addr - 0x00200000]);
        }
        else if (p_addr == 0x1F000024)
        {
            if (!(std::is_same<T, uint32_t>::value))
            {
                std::cout << BOLDRED "memory_read: Tried to read from EXPEVT register with a size != LONGWORD ...!" << RESET << "\n";
                exit(1);
            }

            std::cout << BOLDMAGENTA << "memory_read: Read from the EXPEVT register" << RESET << std::endl;

            return static_cast<uint32_t>(cpu->get_expevt());
        }
        else
        {
            std::cout << BOLDRED "memory_read: Unhandled read at address 0x" << format("{:08X}", p_addr) << RESET << "\n";
            exit(1);
        }

        if (std::is_same<T, uint8_t>::value)
        {
            return static_cast<T>(*from);
        }
        else if (std::is_same<T, uint16_t>::value)
        {
            return static_cast<T>(*from);
        }
        else if (std::is_same<T, uint32_t>::value)
        {
            return static_cast<T>(*from);
        }
    }

    template <typename T>
    void write(uint32_t address, T value, Sh4_Cpu *cpu) {

        // Get MD Bit (Bit #30) from the Status Register
        bool md_bit = cpu->get_md_bit();

        bool p = (address >> 31) & 1;
        bool alt = (address >> 30) & 1;
        bool nc = (address >> 29) & 1;

//#ifdef MEMORY_DEBUG
        std::cout << "memory_write: Writing to 0x" << format("{:08X}", address) << ", P: " << +(p) << " , ALT: " 
        << +(alt) << " , NC: " << +(nc) << std::endl;

        if (address >= 0 && address <= 0x7FFFFFFF)
        {
            std::cout << "             U0/P0 read (";
        }
        else
        if (address >= 0x80000000 && address <= 0x9FFFFFFF)
        {
            std::cout << "             P1 read (";
        }
        else
        if (address >= 0xA0000000 && address <= 0xBFFFFFFF)
        {
            std::cout << "             P2 read (";
        }
        else
        if (address >= 0xC0000000 && address <= 0xDFFFFFFF)
        {
            std::cout << "             P3 read (";
        }
        else
        if (address >= 0xE0000000 && address <= 0xFFFFFFFF)
        {
            std::cout << "             P4 read (";
        }
//#endif

        // Calculate the physical address
        std::uint32_t p_addr = (address & 0x1FFFFFFF);

//#ifdef MEMORY_DEBUG
        std::cout << "Physical: 0x" << format("{:08X}", p_addr) << ")" << std::endl;
//#endif

        if (p_addr >= 0x00000000 && p_addr <= 0x001FFFFF)
        {
            bios[p_addr] = value;
        }
        else if (p_addr >= 0x00200000 && p_addr <= 0x0023FFFF)
        {
            flash[p_addr - 0x00200000] = value;
        }
        else
        {
            std::cout << BOLDRED << "memory_write: Unhandled write at address 0x" << format("{:08X}", p_addr) << " with value 0x";
            
            if (std::is_same<T, uint8_t>::value)
            {
                std::cout << format("{:02X}", value);
            }
            else if (std::is_same<T, uint16_t>::value)
            {
                std::cout << format("{:04X}", value);
            }
            else if (std::is_same<T, uint32_t>::value)
            {
                std::cout << format("{:08X}", value);
            }

            std::cout << RESET << std::endl;

            exit(1);
        }
    }

};
