#include <cassert>

#include "Mapper0.h"

Mapper0::Mapper0(std::unique_ptr<Cartridge> c)
    : Mapper(std::move(c)),
      isMirrored(cartridge->prgRom.size() == 16 * 1024) {}

uint8_t Mapper0::cpuRead(uint16_t addr) {
    uint8_t data = 0;

    if (addr >= 0x8000) {
        // PRG ROM
        addr = (addr & (isMirrored ? 0xBFFF : 0xFFFF));
        data = cartridge->prgRom[addr - 0x8000];
    }

    return data;
}

void Mapper0::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x8000) {
        // PRG ROM
        addr = (addr & (isMirrored ? 0xBFFF : 0xFFFF));
        cartridge->prgRom[addr - 0x8000] = data;
    }
}

uint8_t Mapper0::ppuRead(uint16_t addr) {
    assert(addr >= 0 && addr < 0x2000);
    return cartridge->chrRom[addr];
}

void Mapper0::ppuWrite(uint16_t addr, uint8_t data) {
    assert(addr >= 0 && addr < 0x2000);
    cartridge->chrRom[addr] = data;
}
