#include <utility>

#include "Cartridge.h"

Cartridge::Cartridge(std::uint8_t prgBanks,
                     std::uint8_t chrBanks,
                     std::uint8_t mapperNum,
                     Mirroring mirroring,
                     std::vector<std::uint8_t> prgRom,
                     std::vector<std::uint8_t> chrRom)
    : prgBanks(prgBanks),
      chrBanks(chrBanks),
      mapperNum(mapperNum),
      mirroring(mirroring),
      prgRom(std::move(prgRom)),
      chrRom(std::move(chrRom)) {}
