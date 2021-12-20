#ifndef OCFBNJ_NES_MAPPER1_H
#define OCFBNJ_NES_MAPPER1_H

#include <array>

#include "Mapper.h"
#include "literals.h"

class Mapper1 : public Mapper {
public:
    using Mapper::Mapper;

    uint8_t cpuRead(uint16_t addr) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;

    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;

    void reset() override;

    [[nodiscard]] Mirroring mirroring() const override;

private:
    std::array<uint8_t, 8_kb> prgRam{};

    uint8_t loadRegister = 0;
    uint8_t controlRegister = 0x1C;

    uint8_t chrBank0 = 0;
    uint8_t chrBank1 = 0;
    uint8_t prgBank = 0;
};

#endif // OCFBNJ_NES_MAPPER1_H
