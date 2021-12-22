#ifndef OCFBNJ_NES_MAPPER3_H
#define OCFBNJ_NES_MAPPER3_H

#include "Mapper.h"

class Mapper3 : public Mapper {
public:
    using Mapper::Mapper;

    void serialize(std::ostream &os) override;
    void deserialize(std::istream &is) override;

    uint8_t cpuRead(uint16_t addr) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;

    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;

    void reset() override;

private:
    uint8_t bankSelect = 0;
};

#endif // OCFBNJ_NES_MAPPER3_H
