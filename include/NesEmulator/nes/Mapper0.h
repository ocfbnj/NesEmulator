#ifndef OCFBNJ_NES_MAPPER0_H
#define OCFBNJ_NES_MAPPER0_H

#include "Mapper.h"

// Mapper0 essentially means "no mapper" that CPU reads both CHR and PRG ROM as is.
class Mapper0 : public Mapper {
public:
    explicit Mapper0(std::unique_ptr<Cartridge> cartridge);

    uint8_t cpuRead(uint16_t addr) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;

    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;
};

#endif // OCFBNJ_NES_MAPPER0_H
