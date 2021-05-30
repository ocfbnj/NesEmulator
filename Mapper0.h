#ifndef MAPPER0_H
#define MAPPER0_H

#include <cstdint>

#include "Mapper.h"

// Mapper0 essentially means "no mapper" that CPU reads both CHR and PRG ROM as is.
class Mapper0 : public Mapper {
public:
    explicit Mapper0(std::unique_ptr<Cartridge> cartridge);

    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;

private:
    bool isMirrored;
};

#endif
