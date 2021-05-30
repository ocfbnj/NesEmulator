#include "Mapper0.h"
#include "literals.h"

Mapper0::Mapper0(std::unique_ptr<Cartridge> c)
    : Mapper(std::move(c)),
      isMirrored(cartridge->prgRom.size() == 16_kb) {}

uint8_t Mapper0::read(uint16_t addr) {
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

void Mapper0::write(uint16_t addr, uint8_t data) {
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
