#include <utility>

#include "Bus.h"
#include "literals.h"

Bus::Bus(std::unique_ptr<Mapper> mapper)
    : ram(static_cast<const int>(2_kb)),
      mapper(std::move(mapper)) {}

uint8_t Bus::read(uint16_t addr) {
    if (addr < 0x2000) {
        // CPU RAM
        return ram[addr & 0x07FF];
    } else if (addr < 0x4020) {
        // TODO IO Registers
    } else if (addr < 0x6000) {
        // Expansion Rom
    } else {
        // Save RAM and PRG ROM that stored in cartridge
        return mapper->read(addr);
    }

    return 0;
}

void Bus::write(uint16_t addr, uint8_t data) {
    if (addr < 0x2000) {
        // CPU RAM
        ram[addr & 0x07FF] = data;
    } else if (addr < 0x4020) {
        // TODO IO Registers
    } else if (addr < 0x6000) {
        // Expansion Rom
    } else {
        // Save RAM and PRG ROM that stored in cartridge
        mapper->write(addr, data);
    }
}

uint16_t Bus::read16(uint16_t addr) {
    uint16_t lo = read(addr);
    uint16_t hi = read(addr + 1);

    return (hi << 8) | lo;
}

void Bus::write16(uint16_t addr, uint16_t data) {
    write(addr, data & 0xFF);
    write(addr + 1, (data >> 8) & 0xFF);
}
