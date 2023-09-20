#pragma once

#include <cstdint>

#define	SR_INITIAL_VALUE		0b01110000000000000000000011110000
#define SR						status_register
#define SR_RB_BIT				((SR) & (1u << 29))
#define	FPSCR_INITIAL_VALUE		0b00000000000001000000000000000001
#define UNDEFINED_REG_VAL		(static_cast<uint32_t>(rand()) | (static_cast<uint32_t>(rand()) << 16))

class Sh4_Cpu {

private:
	/*
		The SEGA Dreamcast's CPU is a Hitachi SH7750 (SH-4), a 32 bit RISC CPU:

		* 200 MHz/360 MIPS at 3.3V I/O, 1.8V internal
		* 16 x 32-bit general purpose registers
		* 32 x 32-bit single-precision floating point registers
		* 16-bit fixed instruction length for high code density
		* 5 stage pipeline
		* On-chip cache, 8KB instruction and 16KB data
	*/
	
	/*
		General Registers
	*/

	/*
		The SH4 architecture has 16 general registers, denoted as R0 to R15.
		However, these registers are divided into two "banks": bank 0 and bank 1.
		The RB (Register Bank) bit in the status register (SR) controls which bank
		is currently accessible as the general registers.

		The "registers_" array holds:
		R0_BANK0 through R7_BANK0 and R8-R15 registers
	*/
	std::uint32_t registers_[16];

	/*
		The bank1_registers array holds R0_BANK1 through R7_BANK1
	*/
	std::uint32_t bank1_registers[8];

	/*
		The "registers" pointer array is defined like this:

		R0-R7 point to the bank specified by the RB bit.
		
		NOTE: If an operation modifies the RB bit, run the register remapping
		function "remap_banking_registers".

		R8-R15 always point to registers_[8] through registers_[15]
	*/
	std::uint32_t *registers[16];

	/*
		Control Registers
	*/

	/*
		Status Register (SR)

		Initial state is:
		MD Bit = 1 (Privileged Mode)
		RB Bit = 1 (Selected Bank 1)
		BL Bit = 1 (Interrupt Requests switch the Processor to 'Reset' State)
		FD Bit = 0 (FPU Disable Bit, cleared to 0 after a Reset)
		IMASK Bits = 1111 (Masked Interrupts)

		Rest are 0, on hardware some of them are in an undefined state; up to the
		BIOS to initialize the values properly.
	*/
	std::uint32_t status_register;
	
	/*
		Saved Status Register (SSR)

		In the event of an exception or interrupt, the status register gets saved in the SSR.
	*/
	std::uint32_t saved_status_register;

	/*
		Saved Program Counter (SPC)

		In the event of an exception or interrupt, the current PC gets saved in the SPC.
	*/
	std::uint32_t saved_pc;

	/*
		Global Base Register (GBR)

		Used as the base address for GBR-referencing MOV instructions.
	*/
	std::uint32_t global_base_register;

	/*
		Vector Base Register (VBR)

		In the event of an exception or interrupt, used as branch destination base address.
	*/
	std::uint32_t vector_base_register;

	/*
		Saved General Register 15 (SGR)

		In the event of an exception or interrupt, R15 register contents are stored in SGR.
	*/
	std::uint32_t saved_general_register_15;

	/*
		Debug base register (DBR)

		When the user break debug function is enabled (BRCR.UBDE = 1), DBR is referenced
		as the user break handler branch destination address instead of VBR. 
	*/
	std::uint32_t debug_base_register;

	/*
		System Registers
	*/

	/*
		Multiply-and-accumulate register high (MACH)
		Multiply-and-accumulate register low (MACL)

		Used in MAC instruction.
	*/
	std::uint32_t mach;
	std::uint32_t macl;

	/*
		Procedure Register (PR)

		Return address during a subroutine call gets stored in PR.
		Used by BSR, BSRF, JSR or RTS.
	*/
	std::uint32_t procedure_register;

	/*
		Program Counter (PC)
	*/
	std::uint32_t pc;

	/*
		Floating-point Status/Control Register (FPSCR)

		Initial state is:
		RM bit pair (Rounding Mode) = 01 (Round to zero)
		DN bit (Denormalization mode) = 1 (A denormalized number is treated as zero)

		Rest are 0.
	*/
	std::uint32_t fpscr;

	/*
		Floating-point Communication Register (FPUL)

		Data transfer between FPU registers and CPU registers is carried out via the FPUL register. 
	*/
	std::uint32_t fpul;

	/*
		Exception Registers
	*/

	/*
		Exception event register

		Initial Values:

		0x00000000	(Power-on Reset)
		or
		0x00000020 (Manual Reset)
	*/
	std::uint32_t expevt;

public:

	Sh4_Cpu();
	~Sh4_Cpu();

	void remap_banking_registers();
	std::uint32_t get_register(std::uint8_t index);
	void set_register(std::uint8_t index, std::uint32_t value);
	std::uint32_t get_upper_register(std::uint8_t index);
	void set_upper_register(std::uint8_t index, std::uint32_t value);
	std::uint32_t get_bank0_register(std::uint8_t index);
	void set_bank0_register(std::uint8_t index, std::uint32_t value);
	std::uint32_t get_bank1_register(std::uint8_t index);
	void set_bank1_register(std::uint8_t index, std::uint32_t value);

	bool get_md_bit();

	void print_registers();

	void set_pc(std::uint32_t pc_);
	std::uint32_t get_pc();
	
	void set_expevt(std::uint32_t expevt_);
	std::uint32_t get_expevt();
	
	void set_macl(std::uint32_t macl_);
	std::uint32_t get_macl();
};
