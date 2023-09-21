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
    uint16_t opcode = memory->read<uint16_t>(GET_PC(), cpu);
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
                case 0b0011:
                    switch((opcode & 0x00F0) >> 4)
                    {
                        case 0b1000:
                            /*
                                TODO:

                                This deals with cached memory regions, maybe it's important later on?
                            */
                            std::cout << "pref @r" << +(nnnn) << std::endl;
                            std::cout << BOLDYELLOW << "parse_opcode: pref instruction detected, cached address is 0x" << format("{:08X}", GET_REG(nnnn)) << RESET << std::endl;
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

                case 0b1001:
                    if (opcode == 0x0009)
                    {
                        std::cout << "nop" << std::endl;
                    }
                    else
                    {
                        std::cerr << BOLDRED << "parse_opcode: Unimplemented 0b0000 opcode variation 0x" << format("{:02X}", (opcode & 0x000F))
                                << " (0b" << format("{:04b}", (opcode & 0x000F)) << "), subfamily 0b" << format("{:04b}", ((opcode & 0x00F0) >> 4))
                                << RESET << "\n";
                            cpu->print_registers();
                            exit(1);
                    }
                    break;

                case 0b1010:
                    /*
                        To find which STx family of instructions we're dealing with, check the bit pattern
                        at the high nibble of the LSB(yte) of the opcode
                    */
                    switch ((opcode & 0x00F0) >> 4)
                    {
                        case 0b0001:
                            std::cout << BOLDWHITE << "sts macl, r" << +(nnnn) << "\n";
                            SET_REG(nnnn, cpu->get_macl());
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

            0001nnnnmmmmdddd
        */
        case 0b0001:
            std::cout << "mov.l r" << +(mmmm) << ",@(" << +(dddd << 2) << ",r" << +(nnnn) << ")" << std::endl;
            memory->write<uint32_t>(((dddd << 2) + GET_REG(nnnn)), GET_REG(mmmm), cpu);
            break;

        /*
            Opcode type:

            0010nnnnmmmmxxxx
        */
        case 0b0010:
            switch (opcode & 0x000F)
            {
                case 0b0000:
                    std::cout << BOLDWHITE << "mov.b r" << +(mmmm) << ",@r" << +(nnnn) << RESET << "\n";
                    memory->write(GET_REG(nnnn), (std::uint8_t) GET_REG(mmmm), cpu);
                    break;

                case 0b1000:
                    std::cout << BOLDWHITE << "tst r" << +(mmmm) << ", r" << +(nnnn) << "\n";
                    SET_TBIT(GET_REG(mmmm) & GET_REG(nnnn) ? 0 : 1);
                    break;

                case 0b1010:
                    std::cout << BOLDWHITE << "xor r" << +(mmmm) << ", r" << +(nnnn) << "\n";
                    SET_REG(nnnn, GET_REG(mmmm) ^ GET_REG(nnnn));
                    break;

                case 0b1110:
                {
                    std::cout << BOLDWHITE << "mulu.w r" << +(mmmm) << ", r" << +(nnnn) << "\n";
                    std::uint32_t macl_ = (std::uint32_t) ((GET_REG(mmmm) & 0xFFFF) * (GET_REG(nnnn) & 0xFFFF));
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
                case 0b00000001:
                    std::cout << BOLDWHITE << "shlr r" << +(nnnn) << "\n";
                    SET_TBIT(GET_REG(nnnn) & 0x00000001);
                    SET_REG(nnnn, (GET_REG(nnnn) >> 1));
                    break;

                case 0b00000101:
                    std::cout << BOLDWHITE << "rotr r" << +(nnnn) << "\n";
                    SET_TBIT(GET_REG(nnnn) & 0x00000001);
                    SET_REG(nnnn, (GET_REG(nnnn) >> 1));
                    SET_REG(nnnn, GET_REG(nnnn) | (GET_TBIT() << 31));
                    break;

                case 0b00001001:
                    std::cout << BOLDWHITE << "shlr2 r" << +(nnnn) << "\n";
                    SET_REG(nnnn, GET_REG(nnnn) >> 2);
                    break;

                case 0b00011000:
                    std::cout << BOLDWHITE << "shll8 r" << +(nnnn) << "\n";
                    SET_REG(nnnn, GET_REG(nnnn) << 8);
                    break;
                
                case 0b00100001:
                    std::cout << BOLDWHITE << "shar r" << +(nnnn) << "\n";
                    SET_TBIT(GET_REG(nnnn) & 0x00000001);
                    SET_REG(nnnn, (((std::int32_t) GET_REG(nnnn)) >> 1));
                    break;

                case 0b00101000:
                    std::cout << BOLDWHITE << "shll16 r" << +(nnnn) << "\n";
                    SET_REG(nnnn, GET_REG(nnnn) << 16);
                    break;

                case 0b00101011:
                    std::cout << BOLDWHITE << "jmp @r" << +(nnnn) << "\n";
                    SET_PC(GET_DELAY_PC());
                    SET_DELAY_PC(GET_REG(nnnn));
                    skip_pc_set = true;
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

            std::uint32_t value = memory->read<uint32_t>((GET_REG(mmmm) + (dddd << 2)), cpu);

            SET_REG(nnnn, value);
            break;
        }


        /*
            Opcode type:

            0b0100nnnnmmmmxxxx
        */
        case 0b0110:
            switch (opcode & 0x000F)
            {
                case 0b0011:
                    std::cout << BOLDWHITE << "mov r" << +(mmmm) << ", r" << +(nnnn) << "\n";
                    SET_REG(nnnn, GET_REG(mmmm));
                    break;

                case 0b1001:
                    std::cout << BOLDWHITE << "swap.w r" << +(mmmm) << ", r" << +(nnnn) << "\n";
                    SET_REG(nnnn, (GET_REG(mmmm) >> 16) | (GET_REG(mmmm) << 16));
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

            0111nnnniiiiiiii
        */
        case 0b0111:
        {
            std::cout << "add #" << +((std::int32_t) imm) << ",r" << +(nnnn) << std::endl;

            SET_REG(nnnn, GET_REG(nnnn) + ((std::int32_t) imm));

            break;
        }

        /*
            Opcode type:

            0b1000xxxxxxxxxxxx
        */
        case 0b1000:
            switch ((opcode & 0x0F00) >> 8)
            {
                case 0b0001:
                    std::cout << "mov.w r0,@(" << +(dddd << 1) << ",r" << +(mmmm) << ")" << std::endl;
                    memory->write((GET_REG(mmmm) + (dddd << 1)), (std::uint16_t) (GET_REG(0) & 0xFFFF), cpu);
                    break;

                case 0b0101:
                    std::cout << "mov.w @(" << +(dddd << 1) << ",r" << +(mmmm) << "),r0" << std::endl;
                    SET_REG(0, ((dddd << 1) + GET_REG(mmmm)));
                    break;

                case 0b1011:
                {
                    std::uint32_t pc_ = (dddddddd * 2) + GET_PC() + 4;
                    std::cout << BOLDWHITE << "bf 0x" << format("{:08X}", pc_) << "\n";

                    if (!GET_TBIT())
                    {
                        SET_PC(pc_);
                        SET_DELAY_PC(GET_PC() + 2);

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

            0b1100xxxxdddddddd
        */
        case 0b1100:
            switch ((opcode & 0x0F00) >> 8)
            {
                case 0b1000:
                    std::cout << "tst #" << +((std::uint8_t) imm) << ", r0" << std::endl;
                    SET_TBIT((GET_REG(0) & imm) ? 0 : 1);
                    break;

                case 0b1011:
                    std::cout << "or #" << +((std::uint8_t) imm) << ", r0" << std::endl;
                    SET_REG(0, GET_REG(0) | ((std::uint8_t) imm));
                    break;

                default:
                    std::cerr << BOLDRED << "parse_opcode: Unimplemented 0b1100 opcode variation 0x" << format("{:02X}", (opcode & 0x0F00) >> 8) << " (0b" << format("{:04b}", (opcode & 0x0F00) >> 8) << "), complete opcode: 0x" << format("{:04X}", opcode) << RESET << "\n";
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
            SET_REG(nnnn, (std::int32_t) imm);
            break;

        default:
            std::cerr << BOLDRED << "Unimplemented opcode: 0x" << format("{:04X}", opcode) << " (Function bits: 0b" << format("{:04b}", function) << ")" << RESET << "\n";
            cpu->print_registers();
            exit(1);
            break;
    }
    
    if (!skip_pc_set)
    {
        SET_PC(GET_DELAY_PC());
        SET_DELAY_PC(GET_DELAY_PC() + 2);
    }
}
