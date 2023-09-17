#include <memory/memory.hh>
#include <lucid.hh>
#include <iostream>
#include <fstream>

Memory :: Memory()
{
    // Boot ROM (2MB)
    memory_map.resize(0x00200000, 0);  // 0x00200000 = 2MB

    // Flash ROM (256K)
    memory_map.resize(0x00240000, 0);  // 0x00240000 = 2MB + 256K

    // Hardware registers area (64MB) - Area 0
    memory_map.resize(0x04200000, 0);  // 0x04200000 = 64MB

    // Video RAM (8MB) - Area 1
    memory_map.resize(0x04600000, 0);  // 0x04600000 = 64MB + 8MB

    // System RAM (16MB) - Area 3
    memory_map.resize(0x05000000, 0);  // 0x05000000 = 64MB + 8MB + 16MB

    // Tile accelerator command input (4MB) - Area 4
    memory_map.resize(0x05400000, 0);  // 0x05400000 = 64MB + 8MB + 16MB + 4MB
}

void Memory :: load_bios(const std::string& bios_path)
{
    std::ifstream bios_file(bios_path, std::ios::binary);

    if (!bios_file.is_open()) {
        std::cerr << BOLDRED << "Failed to open the BIOS file: " << bios_path << RESET << "\n";
        return;
    }
    else
    {
        std::cout << BOLDBLUE << "BIOS file opened successfully...!" << RESET "\n";
    }

    const uint32_t bios_base_addr = 0x00000000;
    const uint32_t bios_end_addr = bios_base_addr + 0x03FFFFFF;
    uint32_t offset = bios_base_addr;

    while (!bios_file.eof() && offset <= bios_end_addr) {
        char byte;
        bios_file.read(&byte, 1);
        memory_map[offset] = static_cast<uint8_t>(byte);
        offset++;
    }

    bios_file.close();
}

std::uint8_t Memory::read(uint32_t address) {
    if (address < memory_map.size()) {
        return memory_map[address];
    } else {
        return 0;
    }
}
