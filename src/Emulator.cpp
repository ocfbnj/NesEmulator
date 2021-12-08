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
    const uint16_t bank = bus.getPPU().backgroundPatternAddr();
    const auto& vRam = bus.vRam();

    for (int i = 0; i != 960; i++) {
        renderBackgroundTile(bank, vRam[i], i % 32, i / 32);
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
                drawPixel(drawX, drawY, pixel);
            }
        }
    }
    assert(spriteIndex == 0x40);
}

void Emulator::renderBackgroundTile(uint16_t bank, uint8_t n, int tileX, int tileY) {
    // auto begin = std::next(bus.chrRom().begin(), bank + n * 16);
    // auto end = std::next(begin, 16);
    // std::vector<uint8_t> tile{begin, end};

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
