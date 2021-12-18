#ifndef OCFBNJ_NES_MAPPER1_H
#define OCFBNJ_NES_MAPPER1_H

#include "Mapper.h"

#include <array>

class Mapper1 : public Mapper {
public:
    explicit Mapper1(std::unique_ptr<Cartridge> cartridge);

    uint8_t cpuRead(uint16_t addr) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;

    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;

    [[nodiscard]] Mirroring mirroring() const override;

private:
    std::array<uint8_t, 8 * 1024> prgRam{};

    uint8_t loadRegister{};
    uint8_t controlRegister = 0x1C;

    uint8_t chrBank0{};
    uint8_t chrBank1{};
    uint8_t prgBank{};
};

#endif // OCFBNJ_NES_MAPPER1_H
