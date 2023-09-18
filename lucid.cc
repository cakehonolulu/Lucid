#include <memory/memory.hh>
#include <cpu/cpu.hh>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int main(int argc, char **argv)
{
    const std::string bios_arg = "-bios";
    std::string bios_file;

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
                }
                else
                {
                    std::cerr << "No bios file provided\n";
                    return 1;
                }
            }
        }
    }

    Memory memory;
    memory.load_bios(bios_file);

    std::cout << "Memory Map Initialized" << std::endl;

    // Initialize CPU
    Cpu cpu(&memory);
    std::cout << "CPU Initialized" << std::endl;

    cpu.run();

    return 0;
}
