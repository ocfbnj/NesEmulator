#ifndef OCFBNJ_NES_MAPPER3_H
#define OCFBNJ_NES_MAPPER3_H

#include <nes/Mapper.h>

class Mapper3 : public Mapper {
public:
    using Mapper::Mapper;

    std::uint8_t cpuRead(std::uint16_t addr) override;
    void cpuWrite(std::uint16_t addr, std::uint8_t data) override;

    std::uint8_t ppuRead(std::uint16_t addr) override;
    void ppuWrite(std::uint16_t addr, std::uint8_t data) override;

    void reset() override;

    void serialize(std::ostream& os) const override;
    void deserialize(std::istream& is) override;

private:
    std::uint8_t bankSelect = 0;
};

#endif // OCFBNJ_NES_MAPPER3_H
