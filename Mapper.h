#ifndef MAPPER_H
#define MAPPER_H

#include <cstdint>
#include <memory>

#include "Cartridge.h"

// Mapper is an interface.
class Mapper {
public:
    static std::unique_ptr<Mapper> create(std::unique_ptr<Cartridge> cartridge);

    explicit Mapper(std::unique_ptr<Cartridge> cartridge);
    virtual ~Mapper() = default;

    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t data) = 0;

protected:
    std::unique_ptr<Cartridge> cartridge;
};

#endif
