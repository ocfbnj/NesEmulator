#include <cassert>

#include <nes/Mapper/Mapper0.h>

std::uint8_t Mapper0::cpuRead(std::uint16_t addr) {
    std::uint32_t mappedAddr = 0;

    if (addr >= 0x8000 && addr <= 0xFFFF) {
        if (prgBanks() == 1) {
            mappedAddr = addr & 0x3FFF;
        } else if (prgBanks() == 2) {
            mappedAddr = addr & 0x7FFF;
        } else {
            assert(0);
        }
    }

    assert(mappedAddr >= 0 && mappedAddr < cartridge.prgRom.size());
    return cartridge.prgRom[mappedAddr];
}

void Mapper0::cpuWrite(std::uint16_t addr, std::uint8_t data) {
    assert(0);
}

std::uint8_t Mapper0::ppuRead(std::uint16_t addr) {
    assert(addr >= 0 && addr < 0x2000);
    return cartridge.chrRom[addr];
}

void Mapper0::ppuWrite(std::uint16_t addr, std::uint8_t data) {
    assert(addr >= 0 && addr < 0x2000);
    cartridge.chrRom[addr] = data;
}
