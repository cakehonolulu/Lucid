#include <cpu/sh4_cpu.hh>
#include <lucid.hh>
#include <iostream>

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

Sh4_Cpu::Sh4_Cpu()
{
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
    pc = 0xA0000000;
    delay_pc = pc + 2;
    fpscr = FPSCR_INITIAL_VALUE;
    fpul = UNDEFINED_REG_VAL;

    expevt = 0x00000000;

    mmucr = 0x00000000;

    ccr = 0x00000000;

    bcr1 = 0x00000000;
    bcr2 = 0x00003FFC;

    wcr1 = 0x77777777;
    wcr2 = 0xFFFEEFFF;

    mcr = 0x00000000;

    rtcor = 0x0000;
    rfcr = 0x0000;

    sdmr = 0x00000000;

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

Sh4_Cpu::~Sh4_Cpu()
{
}

bool Sh4_Cpu::get_md_bit()
{
    return ((status_register >> 29) & 1);
}

void Sh4_Cpu::print_registers()
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
    
    std::cout << "\n" << BOLDYELLOW << "Exception Registers:" << RESET "\n";
    std::cout << "Exception event register (EXPEVT):                           " << BOLDWHITE << "0x" << format("{:08X}", expevt) << RESET << "\n";
}

void Sh4_Cpu::remap_banking_registers()
{
    for (std::uint8_t i = 0; i < 8; i++)
    {
        registers[i] = SR_RB_BIT ? &bank1_registers[i] : &registers_[i];
    }
}

std::uint32_t Sh4_Cpu::get_register(std::uint8_t index)
{
    return *registers[index];
}

void Sh4_Cpu::set_register(std::uint8_t index, std::uint32_t value)
{
    *registers[index] = value;
}

std::uint32_t Sh4_Cpu::get_upper_register(std::uint8_t index)
{
    return registers_[index];
}

void Sh4_Cpu::set_upper_register(std::uint8_t index, std::uint32_t value)
{
    registers_[index] = value;
}

std::uint32_t Sh4_Cpu::get_bank0_register(std::uint8_t index)
{
    return registers_[index];
}

void Sh4_Cpu::set_bank0_register(std::uint8_t index, std::uint32_t value)
{
    registers_[index] = value;
}

std::uint32_t Sh4_Cpu::get_bank1_register(std::uint8_t index)
{
    return bank1_registers[index];
}

void Sh4_Cpu::set_bank1_register(std::uint8_t index, std::uint32_t value)
{
    bank1_registers[index] = value;
}

void Sh4_Cpu::set_pc(std::uint32_t pc_)
{
    pc = pc_;
}

std::uint32_t Sh4_Cpu::get_pc()
{
    return pc;
}

void Sh4_Cpu::set_delay_pc(std::uint32_t delay_pc_)
{
    delay_pc = delay_pc_;
}

std::uint32_t Sh4_Cpu::get_delay_pc()
{
    return delay_pc;
}

void Sh4_Cpu::set_expevt(std::uint32_t expevt_)
{
    expevt = expevt_;
}

std::uint32_t Sh4_Cpu::get_expevt()
{
    return expevt;
}

void Sh4_Cpu::set_macl(std::uint32_t macl_)
{
    macl = macl_;
}

std::uint32_t Sh4_Cpu::get_macl()
{
    return macl;
}

void Sh4_Cpu::set_tbit(std::uint8_t tbit_)
{
    status_register = (status_register & 0xFFFFFFFE) | (tbit_ & 0x01);
}

std::uint8_t Sh4_Cpu::get_tbit()
{
    return (status_register & 0x01);
}

void Sh4_Cpu::set_mmucr(std::uint32_t mmucr_)
{
    mmucr = mmucr_;
}

std::uint32_t Sh4_Cpu::get_mmucr()
{
    return (mmucr);
}

void Sh4_Cpu::set_ccr(std::uint32_t ccr_)
{
    ccr = ccr_;
}

std::uint32_t Sh4_Cpu::get_ccr()
{
    return ccr;
}

void Sh4_Cpu::set_bcr1(std::uint32_t bcr1_)
{
    bcr1 = bcr1_;
}

std::uint32_t Sh4_Cpu::get_bcr1()
{
    return bcr1;
}

void Sh4_Cpu::set_bcr2(std::uint32_t bcr2_)
{
    bcr2 = bcr2_;
}

std::uint32_t Sh4_Cpu::get_bcr2()
{
    return bcr2;
}

void Sh4_Cpu::set_wcr1(std::uint32_t wcr1_)
{
    wcr1 = wcr1_;
}

std::uint32_t Sh4_Cpu::get_wcr1()
{
    return wcr1;
}

void Sh4_Cpu::set_wcr2(std::uint32_t wcr2_)
{
    wcr2 = wcr2_;
}

std::uint32_t Sh4_Cpu::get_wcr2()
{
    return wcr2;
}

void Sh4_Cpu::set_mcr(std::uint32_t mcr_)
{
    mcr = mcr_;
}

std::uint32_t Sh4_Cpu::get_mcr()
{
    return mcr;
}

void Sh4_Cpu::set_sdmr(std::uint16_t sdmr_)
{
    sdmr = sdmr_;
}

std::uint16_t Sh4_Cpu::get_sdmr()
{
    return sdmr;
}

void Sh4_Cpu::set_rfcr(std::uint16_t rfcr_)
{
    rfcr = rfcr_;
}

std::uint16_t Sh4_Cpu::get_rfcr()
{
    return rfcr;
}

void Sh4_Cpu::set_rtcor(std::uint16_t rtcor_)
{
    rtcor = rtcor_;
}

std::uint16_t Sh4_Cpu::get_rtcor()
{
    return rtcor;
}

void Sh4_Cpu::set_rtcsr(std::uint16_t rtcsr_)
{
    rtcsr = rtcsr_;
}

std::uint16_t Sh4_Cpu::get_rtcsr()
{
    return rtcsr;
}

void Sh4_Cpu::set_sb_g1rrc(std::uint32_t sb_g1rrc_)
{
    sb_g1rrc = sb_g1rrc_;
}

std::uint32_t Sh4_Cpu::get_sb_g1rrc()
{
    return sb_g1rrc;
}
