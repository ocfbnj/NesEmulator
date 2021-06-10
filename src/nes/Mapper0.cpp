#include "NesEmulator/literals.h"

#include "Mapper0.h"

Mapper0::Mapper0(std::unique_ptr<Cartridge> c)
    : Mapper(std::move(c)),
      isMirrored(cartridge->prgRom.size() == 16_kb) {}

uint8_t Mapper0::readChrRom(uint16_t addr) {
    // TODO
    return cartridge->chrRom[addr];
}

void Mapper0::writeChrRom(uint16_t addr, uint8_t data) {
    // TODO
    cartridge->chrRom[addr] = data;
}

uint8_t Mapper0::readPrgRom(uint16_t addr) {
    if (addr < 0x8000) {
        // Save RAM
    } else {
        // PRG ROM
        if (isMirrored) {
            return cartridge->prgRom[(addr - 0x8000) & 0x3fff];
        }

        return cartridge->prgRom[addr - 0x8000];
    }

    return 0;
}

void Mapper0::writePrgRom(uint16_t addr, uint8_t data) {
    if (addr < 0x8000) {
        // Save RAM
    } else {
        // PRG ROM
        if (isMirrored) {
            cartridge->prgRom[(addr - 0x8000) & 0x3fff] = data;
        }

        cartridge->prgRom[addr - 0x8000] = data;
    }
}
