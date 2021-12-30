#include <cassert>
#include <chrono>

#include <AL/al.h>
#include <AL/alc.h>

#include <audio_maker/AudioMaker.h>

AudioMaker::AudioMaker(int sampleRate, int channelCount)
    : sampleRate(sampleRate),
      channelCount(channelCount),
      isStop(false),
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

    isStop = false;
    thread = std::thread{&AudioMaker::streamData, this};
}

void AudioMaker::stop() {
    isStop = true;

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
    assert(alGetError() == AL_NO_ERROR);

    while (!isStop) {
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

    alSourceStop(source);
    closeQueue();
    alSourcei(source, AL_BUFFER, 0);
    alDeleteBuffers(buffers.size(), buffers.data());
    assert(alGetError() == AL_NO_ERROR);
}

void AudioMaker::fillQueue() {
    for (int i = 0; i != buffers.size(); i++) {
        fillAndPushBuffer(i);
    }
}

void AudioMaker::closeQueue() {
    ALint count;
    alGetSourcei(source, AL_BUFFERS_QUEUED, &count);

    for (ALint i = 0; i != count; i++) {
        ALuint buffer;
        alSourceUnqueueBuffers(source, 1, &buffer);
    }
}

void AudioMaker::fillAndPushBuffer(int bufferNum) {
    std::vector<std::int16_t> data = getData();

    if (data.size() == 0) {
        return;
    }

    std::uint32_t buffer = buffers[bufferNum];

    alBufferData(buffer, channelFormat, data.data(), data.size() * sizeof(data[0]), sampleRate);
    alSourceQueueBuffers(source, 1, &buffer);
}
