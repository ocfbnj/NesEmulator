#ifndef OCFBNJ_NES_MAPPER1_H
#define OCFBNJ_NES_MAPPER1_H

#include <array>

#include <nes/Mapper.h>
#include <nes/literals.h>

class Mapper1 : public Mapper {
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

private:
    std::array<std::uint8_t, 8_kb> prgRam{};

    std::uint8_t loadRegister = 0;
    std::uint8_t controlRegister = 0x1C;

    std::uint8_t chrBank0 = 0;
    std::uint8_t chrBank1 = 0;
    std::uint8_t prgBank = 0;
};

#endif // OCFBNJ_NES_MAPPER1_H
