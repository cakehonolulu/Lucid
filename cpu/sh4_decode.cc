#include <cpu/sh4_decode.hh>

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

Sh4_Decode::Sh4_Decode(Sh4_Cpu *cpu_, Memory *memory_)
{
    cpu = cpu_;
    memory = memory_;
}

void Sh4_Decode::run()
{
    while (true)
    {
        uint16_t opcode = fetch_opcode();

        parse_opcode(opcode);

        cpu->set_pc(cpu->get_pc() + 2);
    }
}

uint16_t Sh4_Decode::fetch_opcode()
{
    uint16_t opcode = (memory->read(cpu->get_pc() + 1, cpu) << 8) | memory->read(cpu->get_pc(), cpu);
    return opcode;
}

void Sh4_Decode::parse_opcode(uint16_t opcode)
{
    uint8_t function = (opcode >> 12) & 0xF;

    std::int8_t imm = ((std::int8_t) (opcode & 0xFF));
    std::uint8_t nnnn = ((opcode & 0x0F00) >> 8);
    std::uint8_t mmmm = ((opcode & 0x00F0) >> 4);
    std::uint8_t dddd = (std::uint8_t) (opcode & 0x000F >> 0);

    switch (function) {

        /*
            Opcode type:

            0b0100nnnnxxxxxxxx / 0b0100mmmmxxxxxxxx
        */
        case 0b0100:
            switch (opcode & 0x00FF)
            {
                case 0b00001001:
                    std::cout << BOLDWHITE << "shlr2 r" << +(nnnn) << "\n";
                    cpu->set_register(nnnn, cpu->get_register(nnnn) >> 2);
                    break;

                case 0b00011000:
                    std::cout << BOLDWHITE << "shll8 r" << +(nnnn) << "\n";
                    cpu->set_register(nnnn, cpu->get_register(nnnn) << 8);
                    break;
                    
                case 0b00101000:
                    std::cout << BOLDWHITE << "shll16 r" << +(nnnn) << "\n";
                    cpu->set_register(nnnn, cpu->get_register(nnnn) << 16);
                    break;

                default:
                    std::cerr << BOLDRED << "Unimplemented 0b0100 opcode variation : 0x" << format("{:02X}", (opcode & 0x00FF)) << ", complete opcode: 0x" << format("{:04X}", opcode) << RESET << "\n";
                    cpu->print_registers();
                    exit(1);
                    break;
            }
            break;

        /*
            Opcode type:

            0101nnnnmmmmdddd
        */
        case 0b0101:
            std::cout << "mov.l @(" << +(dddd << 2) << ",r" << +(mmmm) << "),r" << +(nnnn) << std::endl;
            // TODO/FIXME:

            break;

        /*
            Opcode type:

            0b0100nnnnmmmmxxxx
        */
        case 0b0110:
            switch (opcode & 0x000F)
            {
                case 0b00001001:
                    std::cout << BOLDWHITE << "swap.w r" << +(mmmm) << ", r" << +(nnnn) << "\n";
                    cpu->set_register(nnnn, (cpu->get_register(mmmm) >> 16) | (cpu->get_register(mmmm) << 16));
                    break;

                default:
                    std::cerr << BOLDRED << "Unimplemented 0b0110 opcode variation : 0x" << format("{:02X}", (opcode & 0x000F)) << ", complete opcode: 0x" << format("{:04X}", opcode) << RESET << "\n";
                    cpu->print_registers();
                    exit(1);
                    break;
            }
            break;

        /*
            Opcode type:

            0b1110nnnniiiiiiii
        */
        case 0b1110:
            std::cout << BOLDWHITE << "mov #" << +(imm) << ", r" << +(nnnn) << "\n";
            cpu->set_register(nnnn, (std::int32_t) imm);
            break;

        default:
            std::cerr << BOLDRED << "Unimplemented opcode: 0x" << format("{:04X}", opcode) << " (Function bits: 0b" << format("{:04b}", function) << ")" << RESET << "\n";
            cpu->print_registers();
            exit(1);
            break;
    }
}
