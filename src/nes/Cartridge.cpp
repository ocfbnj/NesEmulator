#include <utility>

#include "Cartridge.h"

Cartridge::Cartridge(uint8_t prgBanks,
                     uint8_t chrBanks,
                     uint8_t mapperNum,
                     Mirroring mirroring,
                     std::vector<uint8_t> prgRom,
                     std::vector<uint8_t> chrRom)
    : prgBanks(prgBanks),
      chrBanks(chrBanks),
      mapperNum(mapperNum),
      mirroring(mirroring),
      prgRom(std::move(prgRom)),
      chrRom(std::move(chrRom)) {}
