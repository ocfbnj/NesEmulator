#ifndef OCFBNJ_NES_MAPPER4_H
#define OCFBNJ_NES_MAPPER4_H

#include "Mapper.h"

#include <array>

class Mapper4 : public Mapper {
public:
    explicit Mapper4(std::unique_ptr<Cartridge> cartridge);

    uint8_t cpuRead(uint16_t addr) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;

    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;

    [[nodiscard]] Mirroring mirroring() const override;

    [[nodiscard]] bool irqState() const override;
    void irqClear() override;

    void scanline() override;

private:
    std::array<uint8_t, 8 * 1024> prgRam{};

    uint8_t bankSelect{};
    std::array<uint8_t, 8> bankRegister{};

    uint8_t mirror{};
    uint8_t prgRamProtect{};

    uint8_t irqReload{};
    uint8_t irqCounter{};

    bool irqEnable = false;
    bool irqActive = false;
};

#endif // OCFBNJ_NES_MAPPER4_H
