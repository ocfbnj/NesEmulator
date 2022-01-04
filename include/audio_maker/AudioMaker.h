#ifndef OCFBNJ_AUDIO_MAKER_H
#define OCFBNJ_AUDIO_MAKER_H

#include <array>
#include <atomic>
#include <cstdint>
#include <functional>
#include <thread>
#include <vector>

struct ALCdevice;
struct ALCcontext;

class AudioMaker {
public:
    using GetData = std::function<std::vector<std::int16_t>()>;

    explicit AudioMaker(int sampleRate = 44100, int channelCount = 1);
    ~AudioMaker();

    void setCallback(GetData f);
    void setProcessingInterval(int ms);

    void run();
    void stop();
    void restart();

private:
    void streamData();

    void fillQueue();
    void closeQueue();
    void fillAndPushBuffer(int bufferNum);

    int sampleRate;
    int channelCount;
    int channelFormat;

    ALCdevice* audioDevice;
    ALCcontext* audioContext;
    std::uint32_t source;
    std::array<std::uint32_t, 3> buffers;

    std::thread thread;
    std::atomic<bool> isStop;
    int processingInterval;

    GetData getData;
};

#endif // OCFBNJ_AUDIO_MAKER_H
