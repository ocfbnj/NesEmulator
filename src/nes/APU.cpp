#include <array>
#include <cassert>
#include <cmath>
#include <iostream>

#include <nes/APU.h>

namespace {
constexpr auto CpuFrequency = 1.789773 * 1'000'000;
constexpr auto ApuFrequency = static_cast<std::uint32_t>(CpuFrequency / 2 + 0.5);
constexpr auto FrameCounterFrequency = 240;
constexpr auto FrameCounterPeroid = static_cast<std::uint16_t>(ApuFrequency / FrameCounterFrequency + 0.5);
} // namespace

void APU::clock() {
    stepTimer();
    stepFrameCounter();

    sendSample();
}

void APU::reset() {
}

std::uint8_t APU::apuRead() {
    assert(addr == 0x4015);

    // TODO
    return 0;
}

void APU::apuWrite(std::uint16_t addr, std::uint8_t data) {
    assert(addr >= 0x4000 && addr < 0x4009 || addr >= 0x400A && addr < 0x400D || addr >= 0x400E && addr < 0x4014 || addr == addr == 0x4015);
    switch (addr) {
    default:
    case 0x4000:
        pulse1.writeControl(data);
        break;
    case 0x4002:
        pulse1.writeTimerLo(data);
        break;
    case 0x4003:
        break;
    }
}

void APU::setSampleRate(int rate) {
    sampleRate = rate;
}

void APU::setSampleCallback(SampleCallback callback) {
    sampleCallback = std::move(callback);
}

void APU::serialize(std::ostream& os) const {
}

void APU::deserialize(std::istream& is) {
}

void APU::stepTimer() {
    pulse1.stepTimer();
}

void APU::stepFrameCounter() {
    static std::uint16_t i = 0;

    if ((i % FrameCounterPeroid) == 0) {
        switch (frameCounterMode) {
        case 4:
            switch (frameCounter) {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            default:
                assert(0);
                break;
            }
            break;
        case 5:
            switch (frameCounter) {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            default:
                assert(0);
                break;
            }
            break;
        default:
            assert(0);
            break;
        }

        if (++frameCounter == frameCounterMode) {
            frameCounter = 0;
        }
    }

    if (++i == FrameCounterPeroid) {
        i = 0;
    }
}

void APU::sendSample() {
    assert(sampleRate > 0);
    assert(ApuFrequency > sampleRate);

    static std::uint32_t i = 0;

    std::uint32_t peroid = ApuFrequency / sampleRate;

    if ((i % peroid) == 0) {
        sampleCallback(getOutputSample());
    }

    if (++i == peroid) {
        i = 0;
    }
}

double APU::getOutputSample() {
    return pulse1.output();
}
