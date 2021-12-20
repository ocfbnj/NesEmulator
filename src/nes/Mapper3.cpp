#include <cassert>

#include "Mapper3.h"

Mapper3::Mapper3(std::unique_ptr<Cartridge> cartridge) : Mapper(std::move(cartridge)) {}

uint8_t Mapper3::cpuRead(uint16_t addr) {
    uint32_t mappedAddr = 0;

    if (addr >= 0x8000 && addr <= 0xFFFF) {
        if (prgBanks() == 1) {
            mappedAddr = addr & 0x3FFF;
        } else if (prgBanks() == 2) {
            mappedAddr = addr & 0x7FFF;
        } else {
            assert(0);
        }
    }

    assert(mappedAddr >= 0 && mappedAddr < cartridge->prgRom.size());
    return cartridge->prgRom[mappedAddr];
}

void Mapper3::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        // CNROM only implements the lowest 2 bits,
        // capping it at 32 KiB CHR. Other boards may implement 4 or more bits for larger CHR.
        bankSelect = data & 0b11;
    }
}

uint8_t Mapper3::ppuRead(uint16_t addr) {
    assert(addr >= 0 && addr < 0x2000);

    uint32_t mappedAddr = 0;

    if (addr >= 0 && addr < 0x2000) {
        mappedAddr = bankSelect * 0x2000 + addr;
    }

    assert(mappedAddr >= 0 && mappedAddr < cartridge->chrRom.size());
    return cartridge->chrRom[mappedAddr];
}

void Mapper3::ppuWrite(uint16_t addr, uint8_t data) {
    assert(0);
}
