#include <memory/memory.hh>
#include <lucid.hh>
#include <iostream>
#include <fstream>

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

Memory :: Memory()
{
    bios = new std::uint8_t[2 * 1024 * 1024];			// 2MB
    flash = new std::uint8_t[256 * 1024];				// 256KB
	main_memory = new std::uint8_t[16 * 1024 * 1024];	// 16MB
	vram = new std::uint8_t[8 * 1024 * 1024];			// 8MB
}

Memory::~Memory() {
    delete[] bios;
    delete[] flash;
    delete[] main_memory;
    delete[] vram;
}

void Memory :: load_bios(const std::string& bios_path)
{
	std::ifstream bios_file(bios_path, std::ios::binary);

	if (!bios_file.is_open()) {
		std::cerr << BOLDRED << "Failed to open the BIOS file: " << bios_path << RESET << "\n";
		return;
	}
	else
	{
		std::cout << BOLDBLUE << "BIOS file opened successfully...!" << RESET "\n";
	}

	const uint32_t bios_base_addr = 0x00000000;
	const uint32_t bios_end_addr = bios_base_addr + 0x001FFFFF;
	uint32_t offset = bios_base_addr;

	while (!bios_file.eof() && offset <= bios_end_addr) {
		char byte;
		bios_file.read(&byte, 1);
		bios[offset] = static_cast<uint8_t>(byte);
		offset++;
	}

	bios_file.close();
}

void Memory :: load_flash(const std::string& flash_path)
{
	std::ifstream flash_file(flash_path, std::ios::binary);

	if (!flash_file.is_open()) {
		std::cerr << BOLDRED << "Failed to open the Flash file: " << flash_path << RESET << "\n";
		return;
	}
	else
	{
		std::cout << BOLDBLUE << "Flash file opened successfully...!" << RESET "\n";
	}

	const uint32_t flash_base_addr = 0x00000000;
	const uint32_t flash_end_addr = flash_base_addr + 0x0003FFFF;
	uint32_t offset = flash_base_addr;

	while (!flash_file.eof() && offset <= flash_end_addr) {
		char byte;
		flash_file.read(&byte, 1);
		flash[offset] = static_cast<uint8_t>(byte);
		offset++;
	}

	flash_file.close();
}

std::uint8_t Memory::read(uint32_t address, Sh4_Cpu *cpu) {

	// Get MD Bit (Bit #30) from the Status Register
	bool md_bit = cpu->get_md_bit();

	bool p = (address >> 31) & 1;
	bool alt = (address >> 30) & 1;
	bool nc = (address >> 29) & 1;

	// Calculate the physical address
	std::uint32_t addr = (address & 0x1FFFFFFF);

	if (addr >= 0x00000000 && addr <= 0x001FFFFF)
	{
		return bios[addr];
	}
	else if (addr >= 0x00200000 && addr <= 0x0023FFFF)
	{
		return flash[addr - 0x0023FFFF];
	}
	else
	{
		std::cout << BOLDRED << "memory_read8: Unhandled address 0x" << format("{:08X}", addr) << RESET << "\n";
		exit(1);
	}
}
