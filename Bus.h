#ifndef BUS_H
#define BUS_H

#include <cstdint>
#include <memory>
#include <vector>

#include "Cpu.h"
#include "Mapper.h"

// CPU gets access to memory (including memory-mapped spaces) using the bus.
// See https://bugzmanov.github.io/nes_ebook/chapter_4.html
class Bus {
public:
    explicit Bus(std::unique_ptr<Mapper> mapper);

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    uint16_t read16(uint16_t addr);
    void write16(uint16_t addr, uint16_t data);

private:
    std::vector<uint8_t> ram;
    std::unique_ptr<Mapper> mapper;
};

#endif
