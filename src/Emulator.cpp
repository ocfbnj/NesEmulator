#include <cmath>

#include "NesEmulator/nes/Mapper.h"
#include "NesEmulator/nes/NesFile.h"

#include "Emulator.h"

Emulator::Emulator(std::string_view nesFile)
    : PixelEngine(256, 240, "Nes Emulator", 3),
      bus(Mapper::create(loadNesFile(nesFile))) {
    bus.getPPU().vblankCallback = [this] { render(); };
}

void Emulator::onUpdate(float elapsedTime) {
    // CPU clock frequency is 1.789773 MHz (~559 ns per cycle)
    int cpuCycles = std::floor(elapsedTime * 1000 / 559);

    for (int i = 0; i != cpuCycles; i++) {
        bus.getPPU().clock();
        bus.getPPU().clock();
        bus.getPPU().clock();
        bus.getCPU().clock();
    }
}

void Emulator::render() {
    renderBackground();
}

void Emulator::renderBackground() {
    const uint16_t bank = bus.getPPU().backgroundPatternAddr();
    const auto& vRam = bus.vRam();

    for (int i = 0; i != 960; i++) {
        renderTile(bank, vRam[i], i % 32, i / 32);
    }
}

void Emulator::renderTile(uint16_t bank, uint8_t n, int tileX, int tileY) {
    // auto begin = std::next(bus.chrRom().begin(), bank + n * 16);
    // auto end = std::next(begin, 16);
    // std::vector<uint8_t> tile{begin, end};

    if (tileX == 9 && tileY == 18) {
        tileY = tileY;
    }

    auto it = std::next(bus.chrRom().begin(), bank + n * 16);
    auto tile = &(*it);
    auto palette = bus.getPPU().backgroundPaletteFor(tileX, tileY);

    for (int y = 0; y != 8; y++) {
        uint8_t lo = tile[y];
        uint8_t hi = tile[y + 8];

        for (int x = 0; x != 8; x++) {
            uint8_t index = (((hi >> (7 - x)) & 1) << 1) | ((lo >> (7 - x)) & 1);
            uint8_t colorIndex = palette[index];
            Pixel pixel = SystemPalette[colorIndex];

            drawPixel(tileX * 8 + x, tileY * 8 + y, pixel);
        }
    }
}
