#include "NesEmulator/nes/Mapper.h"
#include "NesEmulator/nes/NesFile.h"

#include "Emulator.h"

Emulator::Emulator(std::string_view nesFile)
    : PixelEngine(256, 240, "Nes Emulator"),
      bus(Mapper::create(loadNesFile(nesFile))) {}

void Emulator::onUpdate() {
    PixelEngine::onUpdate();

    for (int i = 0; i != 240; i++) {
        for (int j = 0; j != 256; j++) {
            uint8_t val = i;
            drawPixel(i, j, Pixel{.r = val, .g = val, .b = val, .a = 255});
        }
    }
}
