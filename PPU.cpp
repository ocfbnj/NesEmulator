#include "PPU.h"

PPU::PPU(Bus& bus) : bus(bus) {}

uint8_t PPU::read() {
    uint16_t addr = address.get();
    incrementAddr();

    uint8_t res = internalDataBuf;

    if (addr < 0x2000) {
        // TODO CHR ROM
    } else if (addr < 0x3000) {
        // TODO PPU RAM
    } else if (addr < 0x3F00) {
        // not expected to be used
    } else {
        // Palette Table
        res = palette[addr - 0x3F00];
    }

    return res;
}

void PPU::writeAddr(uint8_t data) {
    address.update(data);
}

void PPU::writeCtrl(uint8_t data) {
    control.update(data);
}

void PPU::incrementAddr() {
    address.increment(control.addrIncrement());
}
