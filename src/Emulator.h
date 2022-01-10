#ifndef EMULATOR_H
#define EMULATOR_H

#include <condition_variable>
#include <filesystem>
#include <functional>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <audio_maker/AudioMaker.h>
#include <nes/Bus.h>
#include <pixel_engine/PixelEngine.h>

class Emulator : public PixelEngine {
public:
    explicit Emulator(std::string_view nesFile);

    void onBegin() override;
    void onUpdate() override;
    void onEnd() override;

    void onKeyPress(Key key) override;
    void onKeyRelease(Key key) override;

private:
    void initKeyMap();

    void reset();
    void serialize();
    void deserialize();

    void loadGameAchieve();
    void saveGameAchieve();

    void debug();
    void renderFrame(const PPU::Frame& frame);
    void resetAudioMaker();

    void sampleCallback(double sample);
    std::vector<std::int16_t> audioMakerGetData();

    Bus nes;
    std::filesystem::path nesFilePath;

    std::string dump;
    std::unordered_map<Key, std::function<void()>> pressKeyMap;
    std::unordered_map<Key, std::function<void()>> releaseKeyMap;

    AudioMaker audioMaker;
    std::vector<std::int16_t> samples;
    std::condition_variable cond;
    std::mutex mtx;
    bool stop;

#ifdef OCFBNJ_NES_EMULATOR_DEBUG
    std::uint16_t sampleCount = 0;
#endif
};

#endif
