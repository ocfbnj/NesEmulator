#ifndef OCFBNJ_NES_MAPPER0_H
#define OCFBNJ_NES_MAPPER0_H

#include <nes/Mapper.h>

// Mapper0 essentially means "no mapper" that CPU reads both CHR and PRG ROM as is.
class Mapper0 : public Mapper {
public:
    using Mapper::Mapper;

    std::uint8_t cpuRead(std::uint16_t addr) override;
    void cpuWrite(std::uint16_t addr, std::uint8_t data) override;

    std::uint8_t ppuRead(std::uint16_t addr) override;
    void ppuWrite(std::uint16_t addr, std::uint8_t data) override;
};

#endif // OCFBNJ_NES_MAPPER0_H
