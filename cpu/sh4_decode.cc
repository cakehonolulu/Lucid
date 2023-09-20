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
    }
}

uint16_t Sh4_Decode::fetch_opcode()
{
    uint16_t opcode = memory->read<uint16_t>(cpu->get_pc(), cpu);
    return opcode;
}

void Sh4_Decode::parse_opcode(uint16_t opcode)
{
    uint8_t function = (opcode >> 12) & 0xF;

    std::int8_t imm = ((std::int8_t) (opcode & 0xFF));
    std::uint8_t nnnn = ((opcode & 0x0F00) >> 8);
    std::uint8_t mmmm = ((opcode & 0x00F0) >> 4);
    std::uint8_t dddd = (std::uint8_t) ((opcode & 0x000F) >> 0);
    std::uint16_t dddddddd = (std::uint16_t) ((opcode & 0x00FF) >> 0);

    bool skip_pc_set = false;

    switch (function) {

        /*
            Opcode type:

            0010nnnnxxxxxxxx
        */
        case 0b0000:
            switch (opcode & 0x000F)
            {
                case 0b1010:
                    /*
                        To find which STx family of instructions we're dealing with, check the bit pattern
                        at the high nibble of the LSB(yte) of the opcode
                    */
                    switch ((opcode & 0x00F0) >> 4)
                    {
                        case 0b0001:
                            std::cout << BOLDWHITE << "sts macl, r" << +(nnnn) << "\n";
                            cpu->set_register(nnnn, cpu->get_macl());
                            break;

                        default:
                            std::cerr << BOLDRED << "parse_opcode: Unimplemented 0b0000 opcode variation 0x" << format("{:02X}", (opcode & 0x000F))
                                << " (0b" << format("{:04b}", (opcode & 0x000F)) << "), subfamily 0b" << format("{:04b}", ((opcode & 0x00F0) >> 4))
                                << RESET << "\n";
                            cpu->print_registers();
                            exit(1);
                            break;
                    }
                    break;

                default:
                    std::cerr << BOLDRED << "parse_opcode: Unimplemented 0b0000 opcode variation 0x" << format("{:02X}", (opcode & 0x000F)) << " (0b" << format("{:04b}", (opcode & 0x000F)) << "), complete opcode: 0x" << format("{:04X}", opcode) << RESET << "\n";
                    cpu->print_registers();
                    exit(1);
                    break;
            }
            break;

        /*
            Opcode type:

            0010nnnnmmmmxxxx
        */
        case 0b0010:
            switch (opcode & 0x000F)
            {
                case 0b1000:
                    std::cout << BOLDWHITE << "tst r" << +(mmmm) << ", r" << +(nnnn) << "\n";
                    cpu->set_tbit(cpu->get_register(mmmm) & cpu->get_register(nnnn) ? 0 : 1);
                    break;

                case 0b1010:
                    std::cout << BOLDWHITE << "xor r" << +(mmmm) << ", r" << +(nnnn) << "\n";
                    cpu->set_register(nnnn, cpu->get_register(mmmm) ^ cpu->get_register(nnnn));
                    break;

                case 0b1110:
                {
                    std::cout << BOLDWHITE << "mulu.w r" << +(mmmm) << ", r" << +(nnnn) << "\n";
                    std::uint32_t macl_ = (std::uint32_t) ((cpu->get_register(mmmm) & 0xFFFF) * (cpu->get_register(nnnn) & 0xFFFF));
                    cpu->set_macl(macl_);
                    break;
                }

                default:
                    std::cerr << BOLDRED << "parse_opcode: Unimplemented 0b0010 opcode variation 0x" << format("{:02X}", (opcode & 0x000F)) << " (0b" << format("{:04b}", (opcode & 0x000F)) << "), complete opcode: 0x" << format("{:04X}", opcode) << RESET << "\n";
                    cpu->print_registers();
                    exit(1);
                    break;
            }

            break;

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
                    std::cerr << BOLDRED << "parse_opcode: Unimplemented 0b0100 opcode variation 0x" << format("{:04X}", (opcode & 0x00FF)) << " (0b" << format("{:08b}", (opcode & 0x00FF)) << "), complete opcode: 0x" << format("{:04X}", opcode) << RESET << "\n";
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
        {
            std::cout << "mov.l @(" << +(dddd << 2) << ",r" << +(mmmm) << "),r" << +(nnnn) << std::endl;

            std::uint32_t value = memory->read<uint32_t>((cpu->get_register(mmmm) + (dddd << 2)), cpu);

            cpu->set_register(nnnn, value);
            break;
        }


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
                    std::cerr << BOLDRED << "parse_opcode: Unimplemented 0b0110 opcode variation 0x" << format("{:02X}", (opcode & 0x000F)) << " (0b" << format("{:04b}", (opcode & 0x000F)) << "), complete opcode: 0x" << format("{:04X}", opcode) << RESET << "\n";
                    cpu->print_registers();
                    exit(1);
                    break;
            }
            break;

        /*
            Opcode type:

            0b1000xxxxxxxxxxxx
        */
        case 0b1000:
            switch ((opcode & 0x0F00) >> 8)
            {
                case 0b1011:
                {
                    std::uint32_t pc_ = (dddddddd * 2) + cpu->get_pc() + 4;
                    std::cout << BOLDWHITE << "bf 0x" << format("{:08X}", pc_) << "\n";

                    if (!cpu->get_tbit())
                    {
                        cpu->set_pc(pc_);
                        cpu->set_delay_pc(cpu->get_pc() + 2);

                        skip_pc_set = true;
                    }
                    break;
                }

                default:
                    std::cerr << BOLDRED << "parse_opcode: Unimplemented 0b1000 opcode variation 0x" << format("{:02X}", (opcode & 0x0F00) >> 8) << " (0b" << format("{:04b}", (opcode & 0x0F00) >> 8) << "), complete opcode: 0x" << format("{:04X}", opcode) << RESET << "\n";
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
    
    if (!skip_pc_set)
    {
        cpu->set_pc(cpu->get_delay_pc());
        cpu->set_delay_pc(cpu->get_delay_pc() + 2);
    }
}
