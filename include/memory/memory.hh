#pragma once

#include <string>
#include <vector>
#include <cstdint>

class Memory {

public:

    Memory();

    std::vector<std::uint8_t> memory_map;

    void load_bootrom(const std::string& bootrom_file);
    std::uint8_t read(uint32_t address);
    
};