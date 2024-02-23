#include <memory/memory.hh>
#include <cpu/sh4_cpu.hh>
#include <cpu/sh4_decode.hh>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int main(int argc, char **argv)
{
    const std::string bios_arg = "-bios", flash_arg = "-flash", binary_arg = "-bin";
    std::string bios_file, flash_file, binary_file;
    bool load_bios = false, load_flash = false, load_binary = false;

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <bios_file>\n";
        return 1;
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            if (bios_arg.compare(argv[i]) == 0)
            {
                if (argv[i + 1] != NULL)
                {
                    bios_file = argv[i + 1];
                    i++;
                    load_bios = true;
                }
                else
                {
                    std::cerr << "No bios file provided\n";
                    return 1;
                }
            }
            else if (flash_arg.compare(argv[i]) == 0)
            {
                if (argv[i + 1] != NULL)
                {
                    flash_file = argv[i + 1];
                    i++;
                    load_flash = true;
                }
                else
                {
                    std::cerr << "No flash file provided\n";
                    return 1;
                }
            }
            else if (binary_arg.compare(argv[i]) == 0)
            {
                if (argv[i + 1] != NULL)
                {
                    binary_file = argv[i + 1];
                    i++;
                    load_binary = true;
                }
                else
                {
                    std::cerr << "No binary file provided\n";
                    return 1;
                }
            }
        }
    }

    // Initialize CPU
    Sh4_Cpu cpu;
    std::cout << "CPU Initialized" << std::endl;

    Memory memory;

    if (load_bios)
    {
        memory.load_bios(bios_file);
    }
    else
    {
        std::cout << "In order for Lucid to work we need a BIOS file...!" << std::endl;
        return 1;
    }

    if (load_flash) memory.load_flash(flash_file);

    if (load_binary)
    {
        memory.load_binary(binary_file);
        cpu.set_pc(0x00200000);
        cpu.set_delay_pc(0x00200000 + 2);
    }

    std::cout << "Memory Map Initialized" << std::endl;

    Sh4_Decode decoder(&cpu, &memory);

    decoder.run();

    return 0;
}
