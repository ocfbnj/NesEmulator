#include "Cartridge.h"

Cartridge::Cartridge(std::vector<uint8_t> prgRom, std::vector<uint8_t> chrRom, uint8_t mapper)
    : prgRom(std::move(prgRom)),
      chrRom(std::move(chrRom)),
      mapper(mapper) {}
