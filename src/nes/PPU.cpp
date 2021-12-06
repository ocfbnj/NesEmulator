#include "PPU.h"
#include "Bus.h"

PPU::PPU(Bus& bus) : bus(bus) {}

void PPU::clock() {
    cycles++;

    if (cycles >= 341) {
        cycles -= 341;
        scanline++;

        if (scanline == 241) {
            status.setVblank();
            if (control.isGenerateVblankNMI()) {
                bus.getCPU().nmi();
            }
        }

        if (scanline >= 262) {
            scanline = 0;
            status.resetVblank();
        }
    }
}

uint16_t PPU::spritePatternAddr() const {
    return control.spritePatternAddr();
}

uint16_t PPU::backgroundPatternAddr() const {
    return control.backgroundPatternAddr();
}

uint8_t PPU::readStatus() {
    uint8_t res = status.read();

    // Reading the status register will clear bit 7 and also the address latch used by scroll and addr
    status.resetVblank();
    scroll.resetLatch();
    address.resetLatch();

    return res;
}

uint8_t PPU::readOAMData() const {
    return oamData[oamAddr];
}

uint8_t PPU::readData() {
    uint16_t addr = address.get();
    incrementAddr();

    uint8_t res = internalReadBuf;

    // When reading while the VRAM address is in the range 0-$3EFF (i.e., before the palettes),
    // the read will return the contents of an internal readData buffer.
    if (addr < 0x3F00) {
        // CHR ROM and PPU RAM
        internalReadBuf = bus.ppuRead(addr);
    } else {
        // Palette Table
        res = palette[addr - 0x3F00];
    }

    return res;
}

void PPU::writeCtrl(uint8_t data) {
    bool prev = control.isGenerateVblankNMI();
    control.write(data);

    if (!prev && control.isGenerateVblankNMI() && status.isInVblank()) {
        bus.getCPU().nmi();
    }
}

void PPU::writeMask(uint8_t data) {
    mask.write(data);
}

void PPU::writeOAMAddr(uint8_t data) {
    oamAddr = data;
}

void PPU::writeOAMData(uint8_t data) {
    // Writes will increment oamAddr after the writing
    oamData[oamAddr++] = data;
}

void PPU::writeScroll(uint8_t data) {
    scroll.write(data);
}

void PPU::writeAddr(uint8_t data) {
    address.update(data);
}

void PPU::writeData(uint8_t data) {
    uint16_t addr = address.get();

    if (addr < 0x3F00) {
        // CHR ROM and PPU RAM
        bus.ppuWrite(addr, data);
    } else {
        // Palette Table
        palette[addr - 0x3F00] = data;
    }

    incrementAddr();
}

void PPU::writeOAMDMA(const std::array<uint8_t, 256>& buffer) {
    for (uint8_t data : buffer) {
        oamData[oamAddr++] = data;
    }
}

void PPU::incrementAddr() {
    address.increment(control.addrIncrement());
}
