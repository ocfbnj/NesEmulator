#include "PPU.h"
#include "Bus.h"

PPU::PPU(Bus& bus) : bus(bus) {}

uint8_t PPU::read() {
    uint16_t addr = address.get();
    incrementAddr();

    uint8_t res = internalDataBuf;

    if (addr < 0x3F00) {
        // CHR ROM and PPU RAM
        internalDataBuf = bus.ppuRead(addr);
    } else {
        // Palette Table
        res = palette[addr - 0x3F00];
    }

    return res;
}

void PPU::write(uint8_t data) {
    uint16_t addr = address.get();
    incrementAddr();

    if (addr < 0x3F00) {
        // CHR ROM and PPU RAM
        bus.ppuWrite(addr, data);
    } else {
        // Palette Table
        palette[addr - 0x3F00] = data;
    }
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

uint8_t PPU::getStatus() const {
    return status.get();
}
