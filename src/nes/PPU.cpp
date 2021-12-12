#include <cassert>

#include "Bus.h"
#include "PPU.h"

std::array<PPU::Pixel, 64> PPU::DefaultPalette = {
    Pixel{.r = 0x80, .g = 0x80, .b = 0x80, .a = 0xFF},
    Pixel{.r = 0x00, .g = 0x3D, .b = 0xA6, .a = 0xFF},
    Pixel{.r = 0x00, .g = 0x12, .b = 0xB0, .a = 0xFF},
    Pixel{.r = 0x44, .g = 0x00, .b = 0x96, .a = 0xFF},
    Pixel{.r = 0xA1, .g = 0x00, .b = 0x5E, .a = 0xFF},
    Pixel{.r = 0xC7, .g = 0x00, .b = 0x28, .a = 0xFF},
    Pixel{.r = 0xBA, .g = 0x06, .b = 0x00, .a = 0xFF},
    Pixel{.r = 0x8C, .g = 0x17, .b = 0x00, .a = 0xFF},
    Pixel{.r = 0x5C, .g = 0x2F, .b = 0x00, .a = 0xFF},
    Pixel{.r = 0x10, .g = 0x45, .b = 0x00, .a = 0xFF},
    Pixel{.r = 0x05, .g = 0x4A, .b = 0x00, .a = 0xFF},
    Pixel{.r = 0x00, .g = 0x47, .b = 0x2E, .a = 0xFF},
    Pixel{.r = 0x00, .g = 0x41, .b = 0x66, .a = 0xFF},
    Pixel{.r = 0x00, .g = 0x00, .b = 0x00, .a = 0xFF},
    Pixel{.r = 0x05, .g = 0x05, .b = 0x05, .a = 0xFF},
    Pixel{.r = 0x05, .g = 0x05, .b = 0x05, .a = 0xFF},
    Pixel{.r = 0xC7, .g = 0xC7, .b = 0xC7, .a = 0xFF},
    Pixel{.r = 0x00, .g = 0x77, .b = 0xFF, .a = 0xFF},
    Pixel{.r = 0x21, .g = 0x55, .b = 0xFF, .a = 0xFF},
    Pixel{.r = 0x82, .g = 0x37, .b = 0xFA, .a = 0xFF},
    Pixel{.r = 0xEB, .g = 0x2F, .b = 0xB5, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0x29, .b = 0x50, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0x22, .b = 0x00, .a = 0xFF},
    Pixel{.r = 0xD6, .g = 0x32, .b = 0x00, .a = 0xFF},
    Pixel{.r = 0xC4, .g = 0x62, .b = 0x00, .a = 0xFF},
    Pixel{.r = 0x35, .g = 0x80, .b = 0x00, .a = 0xFF},
    Pixel{.r = 0x05, .g = 0x8F, .b = 0x00, .a = 0xFF},
    Pixel{.r = 0x00, .g = 0x8A, .b = 0x55, .a = 0xFF},
    Pixel{.r = 0x00, .g = 0x99, .b = 0xCC, .a = 0xFF},
    Pixel{.r = 0x21, .g = 0x21, .b = 0x21, .a = 0xFF},
    Pixel{.r = 0x09, .g = 0x09, .b = 0x09, .a = 0xFF},
    Pixel{.r = 0x09, .g = 0x09, .b = 0x09, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF},
    Pixel{.r = 0x0F, .g = 0xD7, .b = 0xFF, .a = 0xFF},
    Pixel{.r = 0x69, .g = 0xA2, .b = 0xFF, .a = 0xFF},
    Pixel{.r = 0xD4, .g = 0x80, .b = 0xFF, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0x45, .b = 0xF3, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0x61, .b = 0x8B, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0x88, .b = 0x33, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0x9C, .b = 0x12, .a = 0xFF},
    Pixel{.r = 0xFA, .g = 0xBC, .b = 0x20, .a = 0xFF},
    Pixel{.r = 0x9F, .g = 0xE3, .b = 0x0E, .a = 0xFF},
    Pixel{.r = 0x2B, .g = 0xF0, .b = 0x35, .a = 0xFF},
    Pixel{.r = 0x0C, .g = 0xF0, .b = 0xA4, .a = 0xFF},
    Pixel{.r = 0x05, .g = 0xFB, .b = 0xFF, .a = 0xFF},
    Pixel{.r = 0x5E, .g = 0x5E, .b = 0x5E, .a = 0xFF},
    Pixel{.r = 0x0D, .g = 0x0D, .b = 0x0D, .a = 0xFF},
    Pixel{.r = 0x0D, .g = 0x0D, .b = 0x0D, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF},
    Pixel{.r = 0xA6, .g = 0xFC, .b = 0xFF, .a = 0xFF},
    Pixel{.r = 0xB3, .g = 0xEC, .b = 0xFF, .a = 0xFF},
    Pixel{.r = 0xDA, .g = 0xAB, .b = 0xEB, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0xA8, .b = 0xF9, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0xAB, .b = 0xB3, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0xD2, .b = 0xB0, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0xEF, .b = 0xA6, .a = 0xFF},
    Pixel{.r = 0xFF, .g = 0xF7, .b = 0x9C, .a = 0xFF},
    Pixel{.r = 0xD7, .g = 0xE8, .b = 0x95, .a = 0xFF},
    Pixel{.r = 0xA6, .g = 0xED, .b = 0xAF, .a = 0xFF},
    Pixel{.r = 0xA2, .g = 0xF2, .b = 0xDA, .a = 0xFF},
    Pixel{.r = 0x99, .g = 0xFF, .b = 0xFC, .a = 0xFF},
    Pixel{.r = 0xDD, .g = 0xDD, .b = 0xDD, .a = 0xFF},
    Pixel{.r = 0x11, .g = 0x11, .b = 0x11, .a = 0xFF},
    Pixel{.r = 0x11, .g = 0x11, .b = 0x11, .a = 0xFF},
};

PPU::PPU(Bus& bus) : bus(bus) {}

void PPU::clock() {
    frameComplete = false;

    // The PPU renders 262 scanlines per frame.
    // Each scanline lasts for 341 PPU clock cycles (113.667 CPU clock cycles; 1 CPU cycle = 3 PPU cycles),
    // with each clock cycle producing one pixel.

    if (scanline >= -1 && scanline < 240) {
        if (scanline == -1) {
            // pre-render scanline
            if (cycle == 1) {
                status.resetSprite0Hit();
                status.resetVblank();
            } else if (cycle >= 280 && cycle < 305) {
                // If rendering is enabled, at the end of vblank,
                // shortly after the horizontal bits are copied from t to v at dot 257,
                // the PPU will repeatedly copy the vertical bits from t to v from dots 280 to 304,
                // completing the full initialization of v from t:
                // v: GHIA.BC DEF..... <- t: GHIA.BC DEF.....
                transferVerticalBits();
            }
        }

        if (cycle >= 1 && cycle < 257 || cycle >= 321 && cycle < 337) {
            updateShifters();

            switch ((cycle - 1) % 8) {
            case 0:
                loadShifters();
                ntByte = bus.ppuRead(0x2000 | vramAddr.reg & 0x0FFF);
                break;
            case 2:
                atByte = bus.ppuRead(0x23C0 | (vramAddr.reg & 0x0C00) | ((vramAddr.reg >> 4) & 0x38) | ((vramAddr.reg >> 2) & 0x07));

                if (vramAddr.coarseY & 0x02) {
                    atByte >>= 4;
                }
                if (vramAddr.coarseX & 0x02) {
                    atByte >>= 2;
                }

                atByte &= 0x03;
                break;
            case 4:
                bgTileByteLSB = bus.ppuRead(control.backgroundPatternAddr() + (ntByte << 4) + vramAddr.fineY + 0);
                break;
            case 6:
                bgTileByteMSB = bus.ppuRead(control.backgroundPatternAddr() + (ntByte << 4) + vramAddr.fineY + 8);
                break;
            case 7:
                incrementHorizontal();
                break;
            default:
                break;
            }
        }

        if (cycle == 256) {
            // If rendering is enabled, the PPU increments the vertical position in v.
            incrementVertical();
        } else if (cycle == 257) {
            // If rendering is enabled, the PPU copies all bits related to horizontal position from t to v:
            // v: ....A.. ...BCDEF <- t: ....A.. ...BCDEF
            transferHorizontalBits();
        } else if (cycle == 338 || cycle == 340) {
            // unused NT fetches
            ntByte = bus.ppuRead(0x2000 | (vramAddr.reg & 0x0FFF));
        }
    } else if (scanline == 240) {
        // post-render scanline
        // do nothing
    } else if (scanline >= 241 && scanline < 261) {
        // vertical blanking lines
        if (scanline == 241 && cycle == 1) {
            status.setVblank();

            if (control.generateNMI()) {
                bus.getCPU().nmi();
            }
        }
    } else {
        assert(0);
    }

    if (showBackground()) {
        uint16_t bit = 0x8000 >> fineX;

        uint8_t bgPixelHi = (patternShifterHi & bit) > 0;
        uint8_t bgPixelLo = (patternShifterLo & bit) > 0;
        uint8_t bgPixel = (bgPixelHi << 1) | bgPixelLo;

        uint8_t bgPaletteHi = (attributeShifterHi & bit) > 0;
        uint8_t bgPaletteLo = (attributeShifterLo & bit) > 0;
        uint8_t bgPalette = (bgPaletteHi << 1) | bgPaletteLo;

        int finalX = cycle - 1;
        int finalY = scanline;

        if (finalX >= 0 && finalX < 256 && finalY >= 0 && finalY < 240) {
            frame.setPixel(finalX, finalY, getColor(bgPalette, bgPixel));
        }
    }

    if (++cycle == 341) {
        // TODO delete
        if (isSprite0Hit()) {
            status.setSprite0Hit();
        }

        cycle = 0;

        if (++scanline == 261) {
            scanline = -1;
            frameComplete = true;
        }
    }
}

uint16_t PPU::spritePatternAddr() const {
    return control.spritePatternAddr();
}

uint8_t PPU::readStatus() {
    uint8_t res = status.read();

    // Reading the status register will clear bit 7 and also the address latch used by PPUSCROLL and PPUADDR
    status.resetVblank();
    latch = 0;

    return res;
}

uint8_t PPU::readOAMData() const {
    return oamData[oamAddr];
}

uint8_t PPU::readData() {
    uint16_t addr = vramAddr.reg;
    incrementAddr();

    uint8_t res = internalReadBuf;

    // When reading while the VRAM address is in the range 0-$3EFF (i.e., before the palettes),
    // the read will return the contents of an internal readData buffer.
    if (addr >= 0x0000 && addr < 0x3F00) {
        internalReadBuf = bus.ppuRead(addr);
    } else if (addr >= 0x3F00 && addr < 0x4000) {
        res = bus.ppuRead(addr);
    } else {
        assert(0);
    }

    return res;
}

uint8_t PPU::readPalette(uint16_t addr) const {
    assert(addr >= 0x3F00 && addr < 0x3F20);
    return paletteTable[addr - 0x3F00] & (mask.isGreyscale() ? 0x30 : 0x3F);
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

bool PPU::renderingEnabled() const {
    return showBackground() || showSprites();
}

const std::array<uint8_t, 256>& PPU::getOamData() const {
    return oamData;
}

const std::array<uint8_t, 32>& PPU::getPaletteTable() const {
    return paletteTable;
}

const PPU::Frame& PPU::getFrame() const {
    return frame;
}

bool PPU::isFrameComplete() const {
    return frameComplete;
}

void PPU::writeCtrl(uint8_t data) {
    //    bool prev = control.generateNMI();
    control.write(data);
    tramAddr.nametableX = data & 1;
    tramAddr.nametableY = (data >> 1) & 1;

    // If the PPU is currently in vertical blank,
    // and the PPUSTATUS ($2002) vblank flag is still set (1),
    // changing the NMI flag in bit 7 of $2000 from 0 to 1 will immediately generate an NMI.
    //    if (status.isInVblank() && !prev && control.generateNMI()) {
    //        bus.getCPU().nmi();
    //    }
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
    if (latch == 0) {
        tramAddr.coarseX = (data >> 3) & 0b1'1111;
        fineX = data & 0b111;
        latch = 1;
    } else {
        tramAddr.coarseY = (data >> 3) & 0b1'1111;
        tramAddr.fineY = data & 0b111;
        latch = 0;
    }
}

void PPU::writeAddr(uint8_t data) {
    if (latch == 0) {
        tramAddr.reg = ((uint16_t)(data & 0b0011'1111)) << 8;
        tramAddr.reg &= ~(0b100'0000);
        latch = 1;
    } else {
        tramAddr.reg |= data;
        vramAddr = tramAddr;
        latch = 0;
    }
}

void PPU::writeData(uint8_t data) {
    uint16_t addr = vramAddr.reg;
    incrementAddr();

    assert(addr >= 0x0000 && addr < 0x4000);
    bus.ppuWrite(addr, data);
}

void PPU::writeOAMDMA(const std::array<uint8_t, 256>& buffer) {
    for (uint8_t data : buffer) {
        oamData[oamAddr++] = data;
    }
}

void PPU::writePalette(uint16_t addr, uint8_t data) {
    assert(addr >= 0x3F00 && addr < 0x3F20);
    paletteTable[addr - 0x3F00] = data;
}

bool PPU::isSprite0Hit() const {
    int y = oamData[0];
    int x = oamData[3];

    return y == scanline && x <= cycle && showSprites();
}

void PPU::incrementAddr() {
    vramAddr.reg += control.addrIncrement();
}

void PPU::incrementHorizontal() {
    // See https://wiki.nesdev.org/w/index.php?title=PPU_scrolling#Coarse_X_increment
    if (renderingEnabled()) {
        if (vramAddr.coarseX == 31) {
            vramAddr.coarseX = 0;
            vramAddr.nametableX = ~vramAddr.nametableX;
        } else {
            vramAddr.coarseX++;
        }
    }
}

void PPU::incrementVertical() {
    // See https://wiki.nesdev.org/w/index.php?title=PPU_scrolling#Y_increment
    if (renderingEnabled()) {
        if (vramAddr.fineY < 7) {
            vramAddr.fineY++;
        } else {
            vramAddr.fineY = 0;

            if (vramAddr.coarseY == 29) {
                vramAddr.coarseY = 0;
                vramAddr.nametableY = ~vramAddr.nametableY;
            } else if (vramAddr.coarseY == 31) {
                vramAddr.coarseY = 0;
            } else {
                vramAddr.coarseY++;
            }
        }
    }
}

void PPU::transferHorizontalBits() {
    if (renderingEnabled()) {
        vramAddr.coarseX = tramAddr.coarseX;
        vramAddr.nametableX = tramAddr.nametableX;
    }
}

void PPU::transferVerticalBits() {
    if (renderingEnabled()) {
        vramAddr.coarseY = tramAddr.coarseY;
        vramAddr.nametableY = tramAddr.nametableY;
        vramAddr.fineY = tramAddr.fineY;
    }
}

PPU::Pixel PPU::getColor(uint8_t palette, uint8_t pixel) {
    if (pixel == 0) {
        if (!((palette >> 3) & 1)) {
            return DefaultPalette[bus.ppuRead(0x3F00)];
        }
    }

    uint8_t index = bus.ppuRead(0x3F00 + ((palette << 2) | pixel));
    assert(index >= 0 && index < 64);

    return DefaultPalette[index];
}

void PPU::loadShifters() {
    patternShifterLo = (patternShifterLo & 0xFF00) | bgTileByteLSB;
    patternShifterHi = (patternShifterHi & 0xFF00) | bgTileByteMSB;

    // for convenient
    attributeShifterLo = (attributeShifterLo & 0xFF00) | ((atByte & 0b01) ? 0xFF : 0x00);
    attributeShifterHi = (attributeShifterHi & 0xFF00) | ((atByte & 0b10) ? 0xFF : 0x00);
}

void PPU::updateShifters() {
    if (showBackground()) {
        patternShifterLo <<= 1;
        patternShifterHi <<= 1;

        attributeShifterLo <<= 1;
        attributeShifterHi <<= 1;
    }
}
