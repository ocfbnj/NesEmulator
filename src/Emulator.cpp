#include <cassert>
#include <unordered_map>

#include "NesEmulator/nes/Mapper.h"
#include "NesEmulator/nes/NesFile.h"

#include "Emulator.h"

Emulator::Emulator(std::string_view nesFile)
    : PixelEngine(256, 240, "Nes Emulator", 3),
      bus(Mapper::create(loadNesFile(nesFile))) {}

void Emulator::onUpdate(float elapsedTime) {
    // CPU clock frequency is 1.789773 MHz
    // PPU clock frequency is three times CPU (~5.369319 MHz)
    // A frame has 341 x 262 = 89,342 clock cycles
    // So NES can output 5,369,319 / 89,342 ~= 60.098 frame per seconds

    checkKeyboard();

    if (freeTime > 0.0f) {
        freeTime -= elapsedTime;
    } else {
        freeTime += (1.0f / 60.0f) - elapsedTime;

        do {
            bus.clock();
        } while (!bus.getPPU().isFrameComplete());

        PPU::Frame frame = bus.getPPU().getFrame();
        for (int x = 0; x < 256; x++) {
            for (int y = 0; y < 240; y++) {
                PPU::Pixel pixel = frame.getPixel(x, y);
                drawPixel(x, y, Pixel{.r = pixel.r, .g = pixel.g, .b = pixel.b, .a = pixel.a});
            }
        }

        renderSprites();
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
        bool behindBackground = (oamData[i + 2] >> 5) & 1;

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
                    if (behindBackground && getPixel(drawX, drawY) != SystemPalette[bus.getPPU().getPaletteTable()[0]]) {
                        continue;
                    }

                    drawPixel(drawX, drawY, pixel);
                }
            }
        }
    }
    assert(spriteIndex == 0x40);
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
