#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>
#include <vector>

// Cartridge represents a NES cartridge.
// It carried two banks of ROM memory: PRG ROM for code and CHR ROM for visual graphics.
// See https://bugzmanov.github.io/nes_ebook/chapter_5.html
struct Cartridge {
    Cartridge(std::vector<uint8_t> prgRom, std::vector<uint8_t> chrRom, uint8_t mapperNum);

    std::vector<uint8_t> prgRom;
    std::vector<uint8_t> chrRom;
    uint8_t mapperNum;
};

#endif
