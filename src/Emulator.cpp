#include <cassert>
#include <cmath>
#include <unordered_map>

#include "NesEmulator/nes/Mapper.h"
#include "NesEmulator/nes/NesFile.h"

#include "Emulator.h"

Emulator::Emulator(std::string_view nesFile)
    : PixelEngine(256, 240, "Nes Emulator", 3),
      bus(Mapper::create(loadNesFile(nesFile))) {
    bus.getPPU().vblankCallback = [this] {
        checkKeyboard();
        render();
    };
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
    if (bus.getPPU().showBackground()) {
        renderBackground();
    }

    if (bus.getPPU().showSprites()) {
        renderSprites();
    }
}

void Emulator::renderBackground() {
    uint8_t scrollX = bus.getPPU().scrollX();
    uint8_t scrollY = bus.getPPU().scrollY();
    assert(scrollX + scrollY == scrollX || scrollX + scrollY == scrollY);

    uint16_t baseNameTableAddr = bus.getPPU().baseNameTableAddr();
    int n = (baseNameTableAddr - 0x2000) / 0x400;
    assert(n >= 0 && n <= 3);

    if (bus.mirroring() == Mirroring::Vertical) {
        int firstNametable;
        int secondNametable;

        if (n == 0) {
            firstNametable = 0;
            secondNametable = 1;
        } else if (n == 1) {
            firstNametable = 1;
            secondNametable = 0;
        } else {
            assert(0);
        }

        renderNametable(firstNametable, {.x1 = scrollX, .y1 = 0, .x2 = 256, .y2 = 240}, -scrollX, 0);
        renderNametable(secondNametable, {.x1 = 0, .y1 = 0, .x2 = scrollX, .y2 = 240}, 256 - scrollX, 0);
    } else {
        // TODO horizontal mirroring
        renderNametable(0);
    }
}

void Emulator::renderSprites() {
    const auto& oamData = bus.getPPU().getOamData();

    uint8_t spriteIndex = 0;
    for (int i = 0; i != oamData.size(); i += 4, spriteIndex++) {
        uint8_t n = oamData[i + 1];
        int tileX = oamData[i + 3];
        int tileY = oamData[i];

        bool flipVertical = (oamData[i + 2] >> 7) & 1;
        bool flipHorizontal = (oamData[i + 2] >> 6) & 1;

        int paletteIndex = oamData[i + 2] & 0b11;
        auto spritePalette = bus.getPPU().spritePalette(paletteIndex);

        uint16_t bank = bus.getPPU().spritePatternAddr();

        auto it = std::next(bus.chrRom().begin(), bank + n * 16);
        auto tile = &(*it);

        for (int y = 0; y != 8; y++) {
            uint8_t lo = tile[y];
            uint8_t hi = tile[y + 8];

            for (int x = 0; x != 8; x++) {
                uint8_t index = (((hi >> (7 - x)) & 1) << 1) | ((lo >> (7 - x)) & 1);
                uint8_t colorIndex = spritePalette[index];
                if (colorIndex == 0) {
                    continue;
                }

                Pixel pixel = SystemPalette[colorIndex];

                int drawX = tileX + (flipHorizontal ? 7 - x : x);
                int drawY = tileY + (flipVertical ? 7 - y : y);

                if (drawX >= 0 && drawX < 256 && drawY >= 0 && drawY < 240) {
                    drawPixel(drawX, drawY, pixel);
                }
            }
        }
    }
    assert(spriteIndex == 0x40);
}

void Emulator::renderNametable(int n, Rect viewport, int shiftX, int shiftY) {
    const uint16_t bank = bus.getPPU().backgroundPatternAddr();
    const uint8_t* nametable = &(*std::next(bus.vRam().begin(), n * 0x400));

    for (int i = 0; i != 960; i++) {
        int tileX = i % 32;
        int tileY = i / 32;

        const uint8_t* tile = &(*std::next(bus.chrRom().begin(), bank + nametable[i] * 16));
        auto palette = bus.getPPU().backgroundPaletteFor(n, tileX, tileY);

        for (int y = 0; y != 8; y++) {
            uint8_t lo = tile[y];
            uint8_t hi = tile[y + 8];

            for (int x = 0; x != 8; x++) {
                uint8_t index = (((hi >> (7 - x)) & 1) << 1) | ((lo >> (7 - x)) & 1);
                uint8_t colorIndex = palette[index];
                Pixel pixel = SystemPalette[colorIndex];

                int drawX = tileX * 8 + x;
                int drawY = tileY * 8 + y;

                if (viewport.isInside(drawX, drawY)) {
                    drawPixel(drawX + shiftX, drawY + shiftY, pixel);
                }
            }
        }
    }
}

void Emulator::checkKeyboard() {
    static const std::unordered_map<int, Joypad::Button> keyMap{
        {GLFW_KEY_J, Joypad::Button::A},
        {GLFW_KEY_K, Joypad::Button::B},
        {GLFW_KEY_SPACE, Joypad::Button::Select},
        {GLFW_KEY_ENTER, Joypad::Button::Start},
        {GLFW_KEY_W, Joypad::Button::Up},
        {GLFW_KEY_S, Joypad::Button::Down},
        {GLFW_KEY_A, Joypad::Button::Left},
        {GLFW_KEY_D, Joypad::Button::Right},
    };

    for (auto [glfwKey, btn] : keyMap) {
        int status = glfwGetKey(getWindow(), glfwKey);
        if (status == GLFW_PRESS) {
            bus.getJoypad().press(btn);
        } else if (status == GLFW_RELEASE) {
            bus.getJoypad().release(btn);
        }
    }
}
