#include <cassert>

#include "Bus.h"
#include "PPU.h"

PPU::PPU(Bus& bus) : bus(bus) {}

void PPU::clock() {
    // The PPU renders 262 scanlines per frame.
    // Each scanline lasts for 341 PPU clock cycles (113.667 CPU clock cycles; 1 CPU cycle = 3 PPU cycles),
    // with each clock cycle producing one pixel.

    if (cycles == 341) {
        if (isSprite0Hit()) {
            status.setSprite0Hit();
        }

        cycles = 0;
        scanline++;

        if (scanline == 241) {
            status.setVblank();
            status.resetSprite0Hit();

            if (control.isGenerateVblankNMI()) {
                vblankCallback();
                bus.getCPU().nmi();
            }
        } else if (scanline == 262) {
            scanline = 0;
            status.resetSprite0Hit();
            status.resetVblank();
        }
    }

    cycles++;
}

uint16_t PPU::baseNameTableAddr() const {
    return control.baseNameTableAddr();
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
        // Addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C
        if (addr == 0x3F10 || addr == 0x3F14 || addr == 0x3F18 || addr == 0x3F1C) {
            addr -= 0x10;
        }
        res = paletteTable[addr - 0x3F00];
    }

    return res;
}

std::array<uint8_t, 4> PPU::backgroundPaletteFor(int nametable, int tileX, int tileY) const {
    uint16_t nametableOffset = nametable * 0x400;
    uint16_t attrTableOffset = nametableOffset + 960;
    uint16_t attrTableIndex = attrTableOffset + tileY / 4 * 8 + tileX / 4;
    assert(attrTableIndex >= attrTableOffset && attrTableIndex <= attrTableOffset + 64);

    uint8_t attrByte = bus.vRam()[attrTableIndex]; // There are 960 bytes for 960 tiles

    uint8_t i = ((((tileY % 4) / 2) << 1) & 0b10) | (((tileX % 4) / 2) & 0b01);
    assert(i >= 0 && i <= 3);

    uint8_t paletteIndex = (attrByte >> (i * 2)) & 0b11;
    assert(paletteIndex >= 0 && paletteIndex <= 3);

    uint8_t paletteStart = paletteIndex * 4;
    return {paletteTable[0], paletteTable[paletteStart + 1], paletteTable[paletteStart + 2], paletteTable[paletteStart + 3]};
}

std::array<uint8_t, 4> PPU::spritePalette(int index) const {
    assert(index >= 0 && index <= 3);
    uint16_t start = 0x11 + index * 4;
    return {0, paletteTable[start], paletteTable[start + 1], paletteTable[start + 2]};
}

bool PPU::showBackground() const {
    return mask.showBackground();
}

bool PPU::showSprites() const {
    return mask.showSprites();
}

uint8_t PPU::scrollX() const {
    return scroll.x;
}
uint8_t PPU::scrollY() const {
    return scroll.y;
}

const std::array<uint8_t, 256>& PPU::getOamData() const {
    return oamData;
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
        // Addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C
        if (addr == 0x3F10 || addr == 0x3F14 || addr == 0x3F18 || addr == 0x3F1C) {
            paletteTable[addr - 0x3F00] = data;
            addr -= 0x10;
        }
        paletteTable[addr - 0x3F00] = data;
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

bool PPU::isSprite0Hit() const {
    int y = oamData[0];
    int x = oamData[3];

    return y == scanline && x <= cycles && showSprites();
}
