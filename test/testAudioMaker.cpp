#include <cmath>
#include <iostream>
#include <numbers>
#include <vector>

#include <audio_maker/AudioMaker.h>

constexpr auto SampleRate = 44100;
constexpr auto SampleSize = 4096;

std::vector<std::int16_t> getData() {
    static double globalTime = 0.0;
    std::vector<std::int16_t> samples(SampleSize);

    for (std::int16_t& sample : samples) {
        sample = static_cast<std::int16_t>(50 * std::sin(440 * 2 * std::numbers::pi * globalTime));
        globalTime += 1.0 / SampleRate;
    }

    return samples;
}

int main() {
    AudioMaker audioMaker{SampleRate, 1};

    audioMaker.setCallback(getData);
    audioMaker.setProcessingInterval(10);
    audioMaker.run();

    std::cout << "Press Enter to stop\n";
    std::cin.get();
    audioMaker.stop();

    std::cout << "Press Enter to restart\n";
    std::cin.get();
    audioMaker.run();

    std::cout << "Press Enter to exit\n";
    std::cin.get();
}
