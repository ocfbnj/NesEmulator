#include <cassert>

#include "Mapper0.h"

Mapper0::Mapper0(std::unique_ptr<Cartridge> cartridge) : Mapper(std::move(cartridge)) {}

uint8_t Mapper0::cpuRead(uint16_t addr) {
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

void Mapper0::cpuWrite(uint16_t addr, uint8_t data) {
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
    cartridge->prgRom[mappedAddr] = data;
}

uint8_t Mapper0::ppuRead(uint16_t addr) {
    assert(addr >= 0 && addr < 0x2000);
    return cartridge->chrRom[addr];
}

void Mapper0::ppuWrite(uint16_t addr, uint8_t data) {
    assert(addr >= 0 && addr < 0x2000);
    cartridge->chrRom[addr] = data;
}
