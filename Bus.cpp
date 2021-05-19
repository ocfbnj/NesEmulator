#include "Bus.h"
#include "literals.h"

Bus::Bus(std::unique_ptr<Mapper> mapper)
    : mapper(std::move(mapper)),
      ram(static_cast<const int>(2_kb)) {}

uint8_t Bus::read(uint16_t addr) {
    if (addr < 0x2000) {
        return ram[addr & 0x07FF];
    } else if (addr < 0x8000) {
        // TODO
    } else {
        return mapper->read(addr);
    }

    return 0;
}

void Bus::write(uint16_t addr, uint8_t data) {
    if (addr < 0x2000) {
        ram[addr & 0x07FF] = data;
    } else if (addr < 0x8000) {
        // TODO
    } else {
        mapper->write(addr, data);
    }
}

uint16_t Bus::read16(uint16_t addr) {
    uint16_t l = read(addr);
    uint16_t h = read(addr + 1);

    return (h << 8) | l;
}

void Bus::write16(uint16_t addr, uint16_t data) {
    write(addr, data & 0xFF);
    write(addr + 1, data >> 8);
}
