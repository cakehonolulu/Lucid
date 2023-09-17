#include <memory/memory.hh>
#include <cpu/cpu.hh>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int main(int argc, char **argv)
{
    const std::string bootrom_arg = "-bootrom";
    std::string bootrom_file;

    if (argc < 2)
    {
        std::cerr << "Usage: ./" << argv[0] << " <bootrom_file>\n";
        return 1;
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            if (bootrom_arg.compare(argv[i]) == 0)
            {
                if (argv[i + 1] != NULL)
                {
                    bootrom_file = argv[i + 1];
                    i++;
                }
                else
                {
                    std::cerr << "No bootrom file provided\n";
                    return 1;
                }
            }
        }
    }

    Memory memory;
    memory.load_bootrom(bootrom_file);

    // Initialize CPU
    Cpu cpu(&memory);
    cpu.run();

    return 0;
}
