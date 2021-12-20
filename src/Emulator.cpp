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

        renderFrame(bus.getPPU().getFrame());
    }
}

void Emulator::renderFrame(const PPU::Frame& frame) {
    for (int x = 0; x < 256; x++) {
        for (int y = 0; y < 240; y++) {
            PPU::Pixel pixel = frame.getPixel(x, y);
            drawPixel(x, y, Pixel{.r = pixel.r, .g = pixel.g, .b = pixel.b, .a = pixel.a});
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

    if (glfwGetKey(getWindow(), GLFW_KEY_R) == GLFW_PRESS) {
        bus.reset();
    }

    for (auto [glfwKey, btn] : keyMap) {
        int status = glfwGetKey(getWindow(), glfwKey);
        if (status == GLFW_PRESS) {
            bus.getJoypad().press(btn);
        } else if (status == GLFW_RELEASE) {
            bus.getJoypad().release(btn);
        }
    }
}
