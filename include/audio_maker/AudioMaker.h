#ifndef OCFBNJ_AUDIO_MAKER_H
#define OCFBNJ_AUDIO_MAKER_H

#include <array>
#include <atomic>
#include <cstdint>
#include <functional>
#include <span>
#include <thread>

struct ALCdevice;
struct ALCcontext;

class AudioMaker {
public:
    using GetData = std::function<std::span<const std::int16_t>()>;

    AudioMaker(int sampleRate = 44100, int channelCount = 1);
    ~AudioMaker();

    void setCallback(GetData f);
    void setProcessingInterval(int ms);

    void run();
    void stop();

private:
    void streamData();

    void fillQueue();
    void fillAndPushBuffer(int bufferNum);

    int sampleRate;
    int channelCount;
    int channelFormat;

    ALCdevice* audioDevice;
    ALCcontext* audioContext;
    std::uint32_t source;
    std::array<std::uint32_t, 3> buffers;

    std::thread thread;
    std::atomic<bool> stoped;
    int processingInterval;

    GetData getData;
};

#endif // OCFBNJ_AUDIO_MAKER_H