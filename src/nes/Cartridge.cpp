#include <utility>

#include "Cartridge.h"

Cartridge::Cartridge(std::vector<uint8_t> prgRom, std::vector<uint8_t> chrRom, uint8_t mapperNum, Mirroring mirroring)
    : prgRom(std::move(prgRom)),
      chrRom(std::move(chrRom)),
      mapperNum(mapperNum),
      mirroring(mirroring) {}
