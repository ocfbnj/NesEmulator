#include <cassert>
#include <chrono>

#include <AL/al.h>
#include <AL/alc.h>

#include <audio_maker/AudioMaker.h>

namespace {
#define alCheck(expr)                        \
    do {                                     \
        expr;                                \
        assert(alGetError() == AL_NO_ERROR); \
    } while (false);

ALfloat listenerPos[] = {0.0, 0.0, 0.0};
ALfloat listenerVel[] = {0.0, 0.0, 0.0};
ALfloat listenerOri[] = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0};
} // namespace

AudioMaker::AudioMaker(int sampleRate, int channelCount)
    : sampleRate(sampleRate),
      channelCount(channelCount),
      isStop(true),
      processingInterval(10) {
    assert(channelCount == 1 || channelCount == 2);
    channelFormat = (channelCount == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    audioDevice = alcOpenDevice(nullptr);
    assert(audioDevice != nullptr);

    audioContext = alcCreateContext(audioDevice, nullptr);
    assert(audioContext != nullptr);

    alcMakeContextCurrent(audioContext);

    alGetError(); // clear error code

    alCheck(alListenerf(AL_GAIN, 100.0f));
    alCheck(alListenerfv(AL_POSITION, listenerPos));
    alCheck(alListenerfv(AL_VELOCITY, listenerVel));
    alCheck(alListenerfv(AL_ORIENTATION, listenerOri));
}

AudioMaker::~AudioMaker() {
    stop();

    alcMakeContextCurrent(nullptr);

    if (audioContext) {
        alcDestroyContext(audioContext);
    }

    if (audioDevice) {
        alcCloseDevice(audioDevice);
    }
}

void AudioMaker::setCallback(GetData f) {
    assert(isStop);
    getData = std::move(f);
}

void AudioMaker::setProcessingInterval(int ms) {
    processingInterval = ms;
}

void AudioMaker::run() {
    alCheck(alGenSources(1, &source));
    alCheck(alSourcei(source, AL_BUFFER, 0));

    isStop = false;
    thread = std::thread{&AudioMaker::streamData, this};
}

void AudioMaker::stop() {
    if (isStop) {
        return;
    }

    isStop = true;

    if (thread.joinable()) {
        thread.join();
    }

    alCheck(alDeleteSources(1, &source));
}

void AudioMaker::restart() {
    stop();
    run();
}

void AudioMaker::streamData() {
    alCheck(alGenBuffers(buffers.size(), buffers.data()));
    alCheck(fillQueue());
    alCheck(alSourcePlay(source));

    while (!isStop) {
        ALint processed = 0;
        alCheck(alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed));

        while (processed--) {
            ALuint buffer;
            alCheck(alSourceUnqueueBuffers(source, 1, &buffer));

            int bufferNum = 0;
            for (int i = 0; i != buffers.size(); ++i) {
                if (buffers[i] == buffer) {
                    bufferNum = i;
                    break;
                }
            }

            fillAndPushBuffer(bufferNum);

            if (processingInterval != 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds{processingInterval});
            }
        }
    }

    alCheck(alSourceStop(source));
    closeQueue();
    alCheck(alSourcei(source, AL_BUFFER, 0));
    alCheck(alDeleteBuffers(buffers.size(), buffers.data()));
}

void AudioMaker::fillQueue() {
    for (int i = 0; i != buffers.size(); i++) {
        fillAndPushBuffer(i);
    }
}

void AudioMaker::closeQueue() {
    ALint count;
    alCheck(alGetSourcei(source, AL_BUFFERS_QUEUED, &count));

    for (ALint i = 0; i != count; i++) {
        ALuint buffer;
        alCheck(alSourceUnqueueBuffers(source, 1, &buffer));
    }
}

void AudioMaker::fillAndPushBuffer(int bufferNum) {
    std::vector<std::int16_t> data;

    if (getData) {
        data = getData();
    }

    ALuint buffer = buffers[bufferNum];

    alCheck(alBufferData(buffer, channelFormat, data.data(), data.size() * sizeof(data[0]), sampleRate));
    alCheck(alSourceQueueBuffers(source, 1, &buffer));
}
