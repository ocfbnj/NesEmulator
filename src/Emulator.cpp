#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numbers>

#include <openssl/evp.h>

#include <nes/NesFile.h>
#include <nes/literals.h>

#include "Emulator.h"

namespace {
std::string getFileSha256(std::string_view filePath) {
    std::ifstream ifs{filePath.data(), std::ifstream::binary | std::ifstream::in};
    if (!ifs) {
        return {};
    }

    std::string fileContent;
    while (!ifs.eof()) {
        char buf[4096];
        ifs.read(buf, sizeof buf);
        fileContent.append(buf, ifs.gcount());
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    EVP_DigestInit(mdctx, EVP_sha256());

    EVP_DigestUpdate(mdctx, fileContent.data(), fileContent.size());

    std::vector<unsigned char> mdValue(EVP_MAX_MD_SIZE, 0);
    unsigned int mdLen;
    EVP_DigestFinal(mdctx, mdValue.data(), &mdLen);
    mdValue.resize(mdLen);

    std::ostringstream oss;
    for (unsigned char value : mdValue) {
        oss << std::hex << +value;
    }

    return oss.str();
}
} // namespace

Emulator::Emulator(std::string_view nesFile)
    : PixelEngine(256, 240, "Nes Emulator", 3),
      nesFilePath(nesFile),
      audioMaker(SampleRate, 1),
      stop(false) {}

void Emulator::onBegin() {
    PixelEngine::onBegin();

    if (!nesFilePath.has_filename()) {
        throw std::runtime_error{nesFilePath.string() + " is not a file"};
    }

    if (!std::filesystem::exists(nesFilePath)) {
        throw std::runtime_error{nesFilePath.string() + " does not exist"};
    }

    std::optional<Cartridge> cartridge = loadNesFile(nesFilePath.string());
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

    loadGameAchieve();
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

    saveGameAchieve();

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

void Emulator::loadGameAchieve() {
    std::string sha256 = getFileSha256(nesFilePath.string());
    std::filesystem::path directory{".NesEmulator/"};
    if (!std::filesystem::exists(directory)) {
        return;
    }

    for (const auto& dirEntry : std::filesystem::directory_iterator{directory}) {
        std::string filename = dirEntry.path().filename().string();

        if (filename.starts_with(sha256)) {
            std::ifstream ifs{dirEntry.path().string(), std::ios_base::binary | std::ios_base::in};
            if (ifs) {
                nes.deserialize(ifs);
                std::cout << "Load from " + dirEntry.path().string() << "\n";

                serialize(); // so we can quickly restore to the startup state
            }

            break;
        }
    }
}

void Emulator::saveGameAchieve() {
    std::string sha256 = getFileSha256(nesFilePath.string());
    std::string filename = nesFilePath.filename().stem().string();
    std::string file = sha256 + "-" + filename;

    std::filesystem::path filePath{".NesEmulator/"};
    if (!std::filesystem::exists(filePath)) {
        std::filesystem::create_directory(filePath);
    }

    filePath /= file;

    std::ofstream ofs{filePath.string(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc};
    if (ofs) {
        nes.serialize(ofs);
        std::cout << "Save to " << filePath.string() << "\n";
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
