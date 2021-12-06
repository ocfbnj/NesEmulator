#include "NesEmulator/nes/Mapper.h"
#include "NesEmulator/nes/NesFile.h"

#include "Emulator.h"

Emulator::Emulator(std::string_view nesFile)
    : PixelEngine(256, 240, "Nes Emulator"),
      bus(Mapper::create(loadNesFile(nesFile))) {}

void Emulator::onUpdate() {
    bus.getCPU().clock();
    render();
}

void Emulator::render() {
    PPU& ppu = bus.getPPU();

    const uint16_t bank = ppu.backgroundPatternAddr();
    const auto& vRam = bus.vRam();
    const auto& chrRom = bus.chrRom();

    for (int i = 0; i != 960; i++) {
        uint8_t n = vRam[i];
        int tileX = i % 32;
        int tileY = i / 32;

        auto begin = std::next(chrRom.begin(), bank + n * 16);
        auto end = std::next(begin, 16);
        std::vector<uint8_t> tile{begin, end};

        for (int y = 0; y != 8; y++) {
            uint8_t hi = tile[y];
            uint8_t lo = tile[y + 8];

            for (int x = 0; x != 8; x++) {
                uint8_t index = (((hi >> (7 - x)) & 1) << 1) | ((lo >> (7 - x)) & 1);
                Pixel pixel{.r = 255, .g = 255, .b = 255, .a = 255};

                switch (index) {
                case 0:
                    pixel = {.r = 0x00, .g = 0x3D, .b = 0xA6, .a = 0xFF};
                    break;
                case 1:
                    pixel = {.r = 0x69, .g = 0xA2, .b = 0xFF, .a = 0xFF};
                    break;
                case 2:
                    pixel = {.r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF};
                    break;
                case 3:
                    pixel = {.r = 0x0D, .g = 0x0D, .b = 0x0D, .a = 0xFF};
                    break;
                default:
                    break;
                }

                drawPixel(tileX * 8 + x, tileY * 8 + y, pixel);
            }
        }
    }
}
