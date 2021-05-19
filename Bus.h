#ifndef BUS_H
#define BUS_H

#include <cstdint>
#include <memory>
#include <vector>

#include "Mapper.h"

class Bus {
public:
    explicit Bus(std::unique_ptr<Mapper> mapper);

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);

    uint16_t read16(uint16_t addr);
    void write16(uint16_t addr, uint16_t data);

private:
    std::unique_ptr<Mapper> mapper;
    std::vector<uint8_t> ram;
};

#endif
