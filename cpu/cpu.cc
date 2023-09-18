#include <cpu/cpu.hh>
#include <lucid.hh>
#include <iostream>

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

Cpu::Cpu(Memory *memory_)
{
    // MMU
    memory = memory_;

    // CPU State

    // Bank 0 and 1 Registers R0-R7 start with undefined values
    for (std::uint8_t i = 0; i < sizeof(registers_); i++)
    {
        registers_[i] = 0x00;
    }

    // Setup R0_BANK1 through R7_BANK1 registers
    for (std::uint8_t i = 0; i < sizeof(bank1_registers); i++)
    {
        bank1_registers[i] = 0x00;
    }

    status_register = SR_INITIAL_VALUE;
    saved_status_register = UNDEFINED_REG_VAL;
    saved_pc = UNDEFINED_REG_VAL;
    global_base_register = UNDEFINED_REG_VAL;
    vector_base_register = UNDEFINED_REG_VAL;
    saved_general_register_15 = UNDEFINED_REG_VAL;
    debug_base_register = UNDEFINED_REG_VAL;

    mach = UNDEFINED_REG_VAL;
    macl = UNDEFINED_REG_VAL;
    procedure_register = UNDEFINED_REG_VAL;
    pc = 0x00000000;
    fpscr = FPSCR_INITIAL_VALUE;
    fpul = UNDEFINED_REG_VAL;

    /*
        Map both banked registers and regular registers to a big register array.

        Using this approach lets us save space, code-wise, and we will only need
        to update the lower 8 pointers whenever there's a SR.RB bit change.
    */
    for (std::uint8_t i = 0; i < 8; i++)
    {
        registers[i] = SR_RB_BIT ? &bank1_registers[i] : &registers_[i];
        registers[i + 8] = &registers_[i];
    }
}

Cpu::~Cpu()
{
}

void Cpu::run()
{
    while (true)
    {
        uint16_t opcode = fetch_opcode();

        parse_opcode(opcode);

        pc += 2;
    }
}

void Cpu::print_registers()
{
    std::cout << BOLDBLUE << "General Registers:" << RESET << std::endl;

    for (int i = 0; i < 8; i++)
    {
        std::cout << "R" << i << "_BANK0: " << BOLDWHITE << "0x" << format("{:08X}", registers_[i])
        << RESET << "        R" << i << "_BANK1: " << BOLDWHITE << "0x" << format("{:08X}", bank1_registers[i]) << RESET;

        if (i + 8 < 10)
        {
            std::cout << "        R" << i + 8 << " : " << BOLDWHITE << "0x" << format("{:08X}", registers_[i]) << RESET << "\n";
        }
        else
        {
            std::cout << "        R" << i + 8 << ": " << BOLDWHITE << "0x" << format("{:08X}", registers_[i]) << RESET << "\n";
        }
    }

    std::cout << "\n" << BOLDGREEN << "Control Registers:" << RESET << "\n";
    std::cout << "Status Register (SR):                                        " << BOLDWHITE << "0x" << format("{:08X}", status_register) << RESET << "\n";
    std::cout << "Saved Status Register (SSR):                                 " << BOLDWHITE << "0x" << format("{:08X}", saved_status_register) << RESET << "\n";
    std::cout << "Saved Program Counter (SPC):                                 " << BOLDWHITE << "0x" << format("{:08X}", saved_pc) << RESET << "\n";
    std::cout << "Global Base Register (GBR):                                  " << BOLDWHITE << "0x" << format("{:08X}", global_base_register) << RESET << "\n";
    std::cout << "Vector Base Register (VBR):                                  " << BOLDWHITE << "0x" << format("{:08X}", vector_base_register) << RESET << "\n";
    std::cout << "Saved General Register 15 (SGR):                             " << BOLDWHITE << "0x" << format("{:08X}", saved_general_register_15) << RESET << "\n";
    std::cout << "Debug Base Register (DBR):                                   " << BOLDWHITE << "0x" << format("{:08X}", debug_base_register) << RESET << "\n";

    std::cout << "\n" << BOLDMAGENTA << "System Registers:" << RESET "\n";
    std::cout << "Multiply-and-accumulate register high (MACH):                " << BOLDWHITE << "0x" << format("{:08X}", mach) << RESET << "\n";
    std::cout << "Multiply-and-accumulate register low (MACL):                 " << BOLDWHITE << "0x" << format("{:08X}", macl) << RESET << "\n";
    std::cout << "Procedure Register (PR):                                     " << BOLDWHITE << "0x" << format("{:08X}", procedure_register) << RESET << "\n";
    std::cout << "Program Counter (PC):                                        " << BOLDWHITE << "0x" << format("{:08X}", pc) << RESET << "\n";
    std::cout << "Floating-point Status/Control Register (FPSCR):              " << BOLDWHITE << "0x" << format("{:08X}", fpscr) << RESET << "\n";
    std::cout << "Floating-point Communication Register (FPUL):                " << BOLDWHITE << "0x" << format("{:08X}", fpul) << RESET << "\n";
}

void Cpu::remap_banking_registers()
{
    for (std::uint8_t i = 0; i < 8; i++)
    {
        registers[i] = SR_RB_BIT ? &bank1_registers[i] : &registers_[i];
    }
}

std::uint32_t Cpu::get_register(std::uint8_t index)
{
    return *registers[index];
}

void Cpu::set_register(std::uint8_t index, std::uint32_t value)
{
    *registers[index] = value;
}

std::uint32_t Cpu::get_upper_register(std::uint8_t index)
{
    return registers_[index];
}

void Cpu::set_upper_register(std::uint8_t index, std::uint32_t value)
{
    registers_[index] = value;
}

std::uint32_t Cpu::get_bank0_register(std::uint8_t index)
{
    return registers_[index];
}

void Cpu::set_bank0_register(std::uint8_t index, std::uint32_t value)
{
    registers_[index] = value;
}

std::uint32_t Cpu::get_bank1_register(std::uint8_t index)
{
    return bank1_registers[index];
}

void Cpu::set_bank1_register(std::uint8_t index, std::uint32_t value)
{
    bank1_registers[index] = value;
}

uint16_t Cpu::fetch_opcode()
{
    uint16_t opcode = (memory->read(pc + 1) << 8) | memory->read(pc);
    return opcode;
}

void Cpu::parse_opcode(uint16_t opcode)
{

    uint8_t function = (opcode >> 12) & 0xF;

    switch (function) {
        case 0b1110:
            std::cout << BOLDWHITE << "mov #" << +((std::int8_t)(opcode & 0xFF)) << ", r" << ((opcode & 0x0F00) >> 8) << "\n";
            //exit(1);
            break;

        default:
            std::cerr << BOLDRED << "Unimplemented opcode: 0x" << format("{:04X}", opcode) << " (Function bits: 0b" << format("{:04b}", function) << ")" << RESET << "\n";
            print_registers();
            exit(1);
            break;
    }
}
