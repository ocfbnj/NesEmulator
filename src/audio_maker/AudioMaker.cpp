#include <cassert>
#include <chrono>

#include <AL/al.h>
#include <AL/alc.h>

#include <audio_maker/AudioMaker.h>

AudioMaker::AudioMaker(int sampleRate, int channelCount)
    : sampleRate(sampleRate),
      channelCount(channelCount),
      stoped(false),
      processingInterval(10) {
    assert(channelCount >= 1 && channelCount < 3);
    channelFormat = (channelCount == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    audioDevice = alcOpenDevice(nullptr);
    assert(audioDevice != nullptr);

    audioContext = alcCreateContext(audioDevice, nullptr);
    assert(audioContext != nullptr);

    alcMakeContextCurrent(audioContext);

    float orientation[] = {
        0.0f,
        0.0f,
        -1.0f,
        0.0f,
        1.0f,
        0.0f,
    };

    alListenerf(AL_GAIN, 100.0f);
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, orientation);
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
    getData = std::move(f);
}

void AudioMaker::setProcessingInterval(int ms) {
    processingInterval = ms;
}

void AudioMaker::run() {
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, 0);

    assert(alGetError() == AL_NO_ERROR);

    stoped = false;
    thread = std::thread{&AudioMaker::streamData, this};
}

void AudioMaker::stop() {
    stoped = true;

    if (thread.joinable()) {
        thread.join();
    }

    alDeleteSources(1, &source);
    assert(alGetError() == AL_NO_ERROR);
}

void AudioMaker::streamData() {
    alGenBuffers(buffers.size(), buffers.data());

    fillQueue();

    alSourcePlay(source);

    while (!stoped) {
        ALint processed = 0;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

        while (processed--) {
            ALuint buffer;
            alSourceUnqueueBuffers(source, 1, &buffer);

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
}

void AudioMaker::fillQueue() {
    for (int i = 0; i != buffers.size(); i++) {
        fillAndPushBuffer(i);
    }
}

void AudioMaker::fillAndPushBuffer(int bufferNum) {
    std::span<const std::int16_t> data = getData();

    std::uint32_t buffer = buffers[bufferNum];

    alBufferData(buffer, channelFormat, data.data(), data.size_bytes(), sampleRate);
    alSourceQueueBuffers(source, 1, &buffer);
}
