#include <unordered_map>

#include "NesEmulator/nes/NesFile.h"

#include "Emulator.h"

Emulator::Emulator(std::string_view nesFile) : PixelEngine(256, 240, "Nes Emulator", 3), nesFile(nesFile) {}

void Emulator::onBegin() {
    std::optional<Cartridge> cartridge = loadNesFile(nesFile);
    if (!cartridge.has_value()) {
        throw std::runtime_error{"Cannot load the NES ROM"};
    }

    nes.insert(std::move(cartridge.value()));
    nes.powerUp();
}

bool Emulator::onUpdate(float elapsedTime) {
    // CPU clock frequency is 1.789773 MHz
    // PPU clock frequency is three times CPU (~5.369319 MHz)
    // A frame has 341 x 262 = 89,342 clock cycles
    // So NES can output 5,369,319 / 89,342 ~= 60.098 frame per seconds

    checkKeyboard();

    if (freeTime > 0.0f) {
        freeTime -= elapsedTime;
        return false;
    }

    freeTime += (1.0f / 60.098f) - elapsedTime;

    do {
        nes.clock();
    } while (!nes.getPPU().isFrameComplete());

    renderFrame(nes.getPPU().getFrame());

    return true;
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

    checkReset();
    checkSerialization();

    for (auto [glfwKey, btn] : keyMap) {
        int status = glfwGetKey(getWindow(), glfwKey);
        if (status == GLFW_PRESS) {
            nes.getJoypad().press(btn);
        } else if (status == GLFW_RELEASE) {
            nes.getJoypad().release(btn);
        }
    }
}

void Emulator::checkReset() {
    static bool pressedReset = false;

    if (int status = glfwGetKey(getWindow(), GLFW_KEY_R); !pressedReset && status == GLFW_PRESS) {
        pressedReset = true;
        nes.reset();
    } else if (status == GLFW_RELEASE) {
        pressedReset = false;
    }
}

void Emulator::checkSerialization() {
    static std::string dump;

    static bool pressedSave = false;
    static bool pressedLoad = false;

    if (int statusI = glfwGetKey(getWindow(), GLFW_KEY_I); !pressedSave && statusI == GLFW_PRESS) {
        pressedSave = true;

        std::ostringstream oss;
        nes.serialize(oss);
        dump = oss.str();
    } else if (statusI == GLFW_RELEASE) {
        pressedSave = false;
    }

    if (int statusL = glfwGetKey(getWindow(), GLFW_KEY_L); !pressedLoad && statusL == GLFW_PRESS) {
        pressedLoad = true;

        if (!dump.empty()) {
            std::istringstream iss{dump};
            nes.deserialize(iss);
        }
    } else if (statusL == GLFW_RELEASE) {
        pressedLoad = false;
    }
}
