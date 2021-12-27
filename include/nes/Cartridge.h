#ifndef OCFBNJ_NES_CARTRIDGE_H
#define OCFBNJ_NES_CARTRIDGE_H

#include <cstdint>
#include <vector>

#include <nes/Mirroring.h>

// Cartridge represents a NES cartridge.
// It carried two banks of ROM memory: PRG ROM for code and CHR ROM for visual graphics.
// See https://bugzmanov.github.io/nes_ebook/chapter_5.html
struct Cartridge {
    std::uint8_t prgBanks;
    std::uint8_t chrBanks;
    std::uint8_t mapperNum;
    Mirroring mirroring;

    std::vector<std::uint8_t> prgRom;
    std::vector<std::uint8_t> chrRom;
};

#endif // OCFBNJ_NES_CARTRIDGE_H
