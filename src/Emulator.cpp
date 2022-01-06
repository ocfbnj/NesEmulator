#include <cassert>
#include <cmath>
#include <numbers>
#include <unordered_map>

#include <nes/NesFile.h>
#include <nes/literals.h>

#include "Emulator.h"

namespace {
const std::unordered_map<Emulator::Key, Joypad::Button> keyMap{
    {Emulator::Key::J, Joypad::Button::A},
    {Emulator::Key::K, Joypad::Button::B},
    {Emulator::Key::Space, Joypad::Button::Select},
    {Emulator::Key::Enter, Joypad::Button::Start},
    {Emulator::Key::W, Joypad::Button::Up},
    {Emulator::Key::S, Joypad::Button::Down},
    {Emulator::Key::A, Joypad::Button::Left},
    {Emulator::Key::D, Joypad::Button::Right},
};
}

Emulator::Emulator(std::string_view nesFile)
    : PixelEngine(256, 240, "Nes Emulator", 3),
      nesFile(nesFile),
      audioMaker(SampleRate, 1),
      stop(false) {}

void Emulator::onBegin() {
    PixelEngine::onBegin();

    std::optional<Cartridge> cartridge = loadNesFile(nesFile);
    if (!cartridge.has_value()) {
        throw std::runtime_error{"Cannot load the NES ROM"};
    }

    nes.insert(std::move(cartridge.value()));

    nes.getAPU().setSampleRate(SampleRate);
    nes.getAPU().setSampleCallback(std::bind(&Emulator::sampleCallback, this, std::placeholders::_1));
    nes.powerUp();

    setFpsLimit(FPS);
    setVsyncEnabled(false);

    audioMaker.setCallback(std::bind(&Emulator::audioMakerGetData, this));
    audioMaker.setProcessingInterval(10);
    audioMaker.run();
}

void Emulator::onUpdate() {
    PixelEngine::onUpdate();

    do {
        nes.clock();
    } while (!nes.getPPU().isFrameComplete());

    renderFrame(nes.getPPU().getFrame());

    debug();
}

void Emulator::onEnd() {
    PixelEngine::onEnd();

    {
        std::lock_guard<std::mutex> lock{mtx};
        stop = true;
    }

    cond.notify_one();
}

void Emulator::onKeyPress(PixelEngine::Key key) {
    PixelEngine::onKeyPress(key);

    static std::string dump;

    if (auto it = keyMap.find(key); it != keyMap.end()) {
        nes.getJoypad().press(it->second);
    }

    if (key == Key::R) {
        nes.reset();
        resetAudioMaker();
    } else if (key == Key::I) {
        std::ostringstream oss;
        nes.serialize(oss);
        dump = oss.str();
    } else if (key == Key::L) {
        if (!dump.empty()) {
            std::istringstream iss{dump};
            nes.deserialize(iss);
        }
    }
}

void Emulator::onKeyRelease(PixelEngine::Key key) {
    PixelEngine::onKeyRelease(key);

    if (auto it = keyMap.find(key); it != keyMap.end()) {
        nes.getJoypad().release(it->second);
    }
}

void Emulator::debug() {
#ifdef OCFBNJ_NES_EMULATOR_DEBUG
    // verify the sampling rate
    static std::uint8_t i = 0;
    if (++i == FPS) {
        i = 0;

        assert(sampleCount == SampleRate);
        sampleCount = 0;
    }
#endif
}

void Emulator::renderFrame(const PPU::Frame& frame) {
    drawPixels(frame.getRawPixels());
}

void Emulator::resetAudioMaker() {
    {
        std::lock_guard<std::mutex> lock{mtx};
        samples.clear();
        stop = true;
    }
    cond.notify_one();

    audioMaker.stop();
    audioMaker.run();

    {
        std::lock_guard<std::mutex> lock{mtx};
        stop = false;
    }
}

void Emulator::sampleCallback(double sample) {
#ifdef OCFBNJ_NES_EMULATOR_DEBUG
    sampleCount++;
#endif

    auto value = static_cast<std::int16_t>(sample * 500);

    {
        std::unique_lock<std::mutex> lock{mtx};

        samples.emplace_back(value);
        if (samples.size() >= SampleCountPerFrame) {
            lock.unlock();
            cond.notify_one();
        }
    }
}

std::vector<std::int16_t> Emulator::audioMakerGetData() {
    std::vector<std::int16_t> data;

    {
        std::unique_lock<std::mutex> lock{mtx};
        cond.wait(lock, [this]() { return samples.size() >= SampleCountPerFrame || stop; });

        samples.swap(data);
    }

    return data;
}
