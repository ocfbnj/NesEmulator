#include <cassert>
#include <cmath>
#include <numbers>

#include <nes/NesFile.h>
#include <nes/literals.h>

#include "Emulator.h"

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

    initKeyMap();

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

    if (auto it = pressKeyMap.find(key); it != pressKeyMap.end()) {
        it->second();
    }
}

void Emulator::onKeyRelease(PixelEngine::Key key) {
    PixelEngine::onKeyRelease(key);

    if (auto it = releaseKeyMap.find(key); it != releaseKeyMap.end()) {
        it->second();
    }
}

void Emulator::initKeyMap() {
    Joypad& joypad1 = nes.getJoypad1();
    Joypad& joypad2 = nes.getJoypad2();

    pressKeyMap = {
        {Emulator::Key::R, [this] { reset(); }},
        {Emulator::Key::I, [this] { serialize(); }},
        {Emulator::Key::L, [this] { deserialize(); }},

        {Emulator::Key::J, [this, &joypad1] { joypad1.press(Joypad::Button::A); }},
        {Emulator::Key::K, [this, &joypad1] { joypad1.press(Joypad::Button::B); }},
        {Emulator::Key::Space, [this, &joypad1] { joypad1.press(Joypad::Button::Select); }},
        {Emulator::Key::Enter, [this, &joypad1] { joypad1.press(Joypad::Button::Start); }},
        {Emulator::Key::W, [this, &joypad1] { joypad1.press(Joypad::Button::Up); }},
        {Emulator::Key::S, [this, &joypad1] { joypad1.press(Joypad::Button::Down); }},
        {Emulator::Key::A, [this, &joypad1] { joypad1.press(Joypad::Button::Left); }},
        {Emulator::Key::D, [this, &joypad1] { joypad1.press(Joypad::Button::Right); }},

        {Emulator::Key::Num1, [this, &joypad2] { joypad2.press(Joypad::Button::A); }},
        {Emulator::Key::Num2, [this, &joypad2] { joypad2.press(Joypad::Button::B); }},
        {Emulator::Key::RightShift, [this, &joypad2] { joypad2.press(Joypad::Button::Select); }},
        {Emulator::Key::RightControl, [this, &joypad2] { joypad2.press(Joypad::Button::Start); }},
        {Emulator::Key::Up, [this, &joypad2] { joypad2.press(Joypad::Button::Up); }},
        {Emulator::Key::Down, [this, &joypad2] { joypad2.press(Joypad::Button::Down); }},
        {Emulator::Key::Left, [this, &joypad2] { joypad2.press(Joypad::Button::Left); }},
        {Emulator::Key::Right, [this, &joypad2] { joypad2.press(Joypad::Button::Right); }},
    };

    releaseKeyMap = {
        {Emulator::Key::J, [this, &joypad1] { joypad1.release(Joypad::Button::A); }},
        {Emulator::Key::K, [this, &joypad1] { joypad1.release(Joypad::Button::B); }},
        {Emulator::Key::Space, [this, &joypad1] { joypad1.release(Joypad::Button::Select); }},
        {Emulator::Key::Enter, [this, &joypad1] { joypad1.release(Joypad::Button::Start); }},
        {Emulator::Key::W, [this, &joypad1] { joypad1.release(Joypad::Button::Up); }},
        {Emulator::Key::S, [this, &joypad1] { joypad1.release(Joypad::Button::Down); }},
        {Emulator::Key::A, [this, &joypad1] { joypad1.release(Joypad::Button::Left); }},
        {Emulator::Key::D, [this, &joypad1] { joypad1.release(Joypad::Button::Right); }},

        {Emulator::Key::Num1, [this, &joypad2] { joypad2.release(Joypad::Button::A); }},
        {Emulator::Key::Num2, [this, &joypad2] { joypad2.release(Joypad::Button::B); }},
        {Emulator::Key::RightShift, [this, &joypad2] { joypad2.release(Joypad::Button::Select); }},
        {Emulator::Key::Num0, [this, &joypad2] { joypad2.release(Joypad::Button::Start); }},
        {Emulator::Key::Up, [this, &joypad2] { joypad2.release(Joypad::Button::Up); }},
        {Emulator::Key::Down, [this, &joypad2] { joypad2.release(Joypad::Button::Down); }},
        {Emulator::Key::Left, [this, &joypad2] { joypad2.release(Joypad::Button::Left); }},
        {Emulator::Key::Right, [this, &joypad2] { joypad2.release(Joypad::Button::Right); }},
    };
}

void Emulator::reset() {
    nes.reset();
    resetAudioMaker();
}

void Emulator::serialize() {
    std::ostringstream oss;
    nes.serialize(oss);
    dump = oss.str();
}

void Emulator::deserialize() {
    if (!dump.empty()) {
        std::istringstream iss{dump};
        nes.deserialize(iss);
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
