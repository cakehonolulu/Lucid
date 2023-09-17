#include <memory/memory.hh>
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

void Memory :: load_bootrom(const std::string& bootrom_file)
{
    std::ifstream bootromFile(bootrom_file, std::ios::binary);

    if (!bootromFile.is_open()) {
        std::cerr << "Failed to open bootrom file: " << bootrom_file << "\n";
        return;
    }
    else
    {
        std::cout << "Bootrom file opened successfully\n";
    }

    // Load bootrom into the memory at the appropriate location
    // Boot ROM is from 0x00000000 to 0x03FFFFFF (2MB)
    const uint32_t bootromBaseAddress = 0x00000000;
    const uint32_t bootromEndAddress = bootromBaseAddress + 0x03FFFFFF;
    uint32_t offset = bootromBaseAddress;

    while (!bootromFile.eof() && offset <= bootromEndAddress) {
        char byte;
        bootromFile.read(&byte, 1);
        memory_map[offset] = static_cast<uint8_t>(byte);
        offset++;
    }

    bootromFile.close();
}

std::uint8_t Memory::read(uint32_t address) {
    // Ensure the address is within the valid memory range
    if (address < memory_map.size()) {
        return memory_map[address];
    } else {
        // Handle out-of-bounds access (you can choose the appropriate behavior)
        // For simplicity, we'll return 0 for out-of-bounds access
        return 0;
    }
}
