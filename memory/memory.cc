#include <memory/memory.hh>
#include <lucid.hh>
#include <iostream>
#include <fstream>
#include <cstring>
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
	memset(main_memory, 0, sizeof(uint8_t) * 16 * 1024 * 1024);
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

void Memory :: dump_ram()
{
	std::ofstream ram_file("ram.bin", std::ios::out | std::ios::binary);

	ram_file.write(reinterpret_cast<const char*>(main_memory), sizeof(uint8_t) * 16 * 1024 * 1024);

    ram_file.close();
}
