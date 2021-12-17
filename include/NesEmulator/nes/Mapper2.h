#ifndef OCFBNJ_NES_MAPPER2_H
#define OCFBNJ_NES_MAPPER2_H

#include "Mapper.h"

class Mapper2 : public Mapper {
public:
    explicit Mapper2(std::unique_ptr<Cartridge> cartridge);

    uint8_t cpuRead(uint16_t addr) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;
    
    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;

private:
    uint8_t bankSelect{};
};

#endif // OCFBNJ_NES_MAPPER2_H
