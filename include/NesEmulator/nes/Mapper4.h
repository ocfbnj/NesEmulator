#ifndef OCFBNJ_NES_MAPPER4_H
#define OCFBNJ_NES_MAPPER4_H

#include <array>

#include "Mapper.h"
#include "literals.h"

class Mapper4 : public Mapper {
public:
    using Mapper::Mapper;

    uint8_t cpuRead(uint16_t addr) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;

    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;

    void reset() override;

    [[nodiscard]] Mirroring mirroring() const override;

    [[nodiscard]] bool irqState() const override;
    void irqClear() override;

    void scanline() override;

private:
    std::array<uint8_t, 8_kb> prgRam{};

    uint8_t bankSelect = 0;
    std::array<uint8_t, 8> bankRegister{};

    uint8_t mirror = 0;
    uint8_t prgRamProtect = 0;

    uint8_t irqReload = 0;
    uint8_t irqCounter = 0;

    bool irqEnable = false;
    bool irqActive = false;
};

#endif // OCFBNJ_NES_MAPPER4_H
