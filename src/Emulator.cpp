#include <cmath>
#include <numbers>
#include <unordered_map>

#include <nes/NesFile.h>

#include "Emulator.h"

Emulator::Emulator(std::string_view nesFile)
    : PixelEngine(256, 240, "Nes Emulator", 3),
      nesFile(nesFile),
      audioMaker(44100, 1) {}

void Emulator::onBegin() {
    std::optional<Cartridge> cartridge = loadNesFile(nesFile);
    if (!cartridge.has_value()) {
        throw std::runtime_error{"Cannot load the NES ROM"};
    }

    nes.insert(std::move(cartridge.value()));
    nes.powerUp();

    // CPU clock frequency is 1.789773 MHz
    // PPU clock frequency is three times CPU (~5.369319 MHz)
    // A frame has 341 x 262 = 89,342 clock cycles
    // So NES can output 5,369,319 / 89,342 ~= 60.098 frame per seconds
    setFpsLimit(60);
    setVsyncEnabled(false);

    audioMaker.setCallback(std::bind(&Emulator::audioMakerGetData, this));
    audioMaker.setProcessingInterval(0);
    audioMaker.run();
}

void Emulator::onUpdate() {
    checkKeyboard();

    do {
        nes.clock();
    } while (!nes.getPPU().isFrameComplete());

    renderFrame(nes.getPPU().getFrame());

    // TODO delete
    for (int i = 0; i != 5000; i++) {
        static double globalTime = 0.0;
        std::int16_t sample = 100 * std::sin(220 * 2 * std::numbers::pi * globalTime);
        {
            std::unique_lock<std::mutex> lock{mtx};
            samples.emplace_back(sample);

            if (samples.size() >= 4096) {
                cond.notify_one();
            }
        }
        globalTime += 1.0 / 44100.0;
    }
}

void Emulator::renderFrame(const PPU::Frame& frame) {
    drawPixels(frame.getRawPixels());
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

std::vector<std::int16_t> Emulator::audioMakerGetData() {
    std::vector<std::int16_t> data;

    {
        std::unique_lock<std::mutex> lock{mtx};
        cond.wait(lock, [this]() { return samples.size() >= 4096; });

        samples.swap(data);
    }

    return data;
}
