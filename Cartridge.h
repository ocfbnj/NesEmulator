#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>
#include <utility>
#include <vector>

// Cartridge represents a NES cartridge.
// See https://wiki.nesdev.com/w/index.php/INES#iNES_file_format
struct Cartridge {
    Cartridge(std::vector<uint8_t> prgRom, std::vector<uint8_t> chrRom, uint8_t mapper)
        : prgRom(std::move(prgRom)),
          chrRom(std::move(chrRom)),
          mapper(mapper) {}

    std::vector<uint8_t> prgRom;
    std::vector<uint8_t> chrRom;
    uint8_t mapper;
};

#endif
