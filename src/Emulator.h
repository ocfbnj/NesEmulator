#ifndef EMULATOR_H
#define EMULATOR_H

#include <condition_variable>
#include <mutex>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <audio_maker/AudioMaker.h>
#include <nes/Bus.h>
#include <pixel_engine/PixelEngine.h>

class Emulator : public PixelEngine {
public:
    explicit Emulator(std::string_view nesFile);

    void onBegin() override;
    void onUpdate() override;

private:
    void renderFrame(const PPU::Frame& frame);
    void checkKeyboard();
    void checkReset();
    void checkSerialization();

    std::span<const std::int16_t> audioMakerGetData();

    Bus nes;
    std::string nesFile;

    AudioMaker audioMaker;
    std::vector<std::int16_t> samples;
    std::condition_variable cond;
    std::mutex mtx;
};

#endif
