#ifndef OCFBNJ_NES_MAPPER4_H
#define OCFBNJ_NES_MAPPER4_H

#include <array>

#include <nes/Mapper.h>
#include <nes/literals.h>

class Mapper4 : public Mapper {
public:
    using Mapper::Mapper;

    std::uint8_t cpuRead(std::uint16_t addr) override;
    void cpuWrite(std::uint16_t addr, std::uint8_t data) override;

    std::uint8_t ppuRead(std::uint16_t addr) override;
    void ppuWrite(std::uint16_t addr, std::uint8_t data) override;

    void reset() override;

    void serialize(std::ostream& os) const override;
    void deserialize(std::istream& is) override;

    Mirroring mirroring() const override;

    bool irqState() const override;
    void irqClear() override;

    void scanline() override;

private:
    std::array<std::uint8_t, 8_kb> prgRam{};

    std::uint8_t bankSelect = 0;
    std::array<std::uint8_t, 8> bankRegister{};

    std::uint8_t mirror = 0;
    std::uint8_t prgRamProtect = 0;

    std::uint8_t irqReload = 0;
    std::uint8_t irqCounter = 0;

    bool irqEnable = false;
    bool irqActive = false;
};

#endif // OCFBNJ_NES_MAPPER4_H
