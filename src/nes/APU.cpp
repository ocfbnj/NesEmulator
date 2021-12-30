#include <array>
#include <cassert>
#include <cmath>

#include <nes/APU.h>

namespace {
constexpr auto CpuFrequency = 1.789773 * 1'000'000;
constexpr auto ApuFrequency = static_cast<std::uint32_t>(CpuFrequency / 2 + 0.5);
constexpr auto FrameCounterFrequency = 240;
constexpr auto FrameCounterPeriod = static_cast<std::uint16_t>(ApuFrequency / FrameCounterFrequency + 0.5);
} // namespace

void APU::clock() {
    stepTimer();
    stepTimer();
    stepTimer();
    stepTimer();

    stepFrameCounter();
    sendSample();
}

void APU::reset() {
}

std::uint8_t APU::apuRead(std::uint16_t addr) {
    assert(addr == 0x4015);

    // TODO
    return 0;
}

void APU::apuWrite(std::uint16_t addr, std::uint8_t data) {
    assert(addr >= 0x4000 && addr < 0x4009 || addr >= 0x400A && addr < 0x400D || addr >= 0x400E && addr < 0x4014 || addr == 0x4015);
    switch (addr) {
    case 0x4000:
        pulse1.writeControl(data);
        break;
    case 0x4002:
        pulse1.writeTimerLo(data);
        break;
    case 0x4003:
        pulse1.writeTimerHi(data);
    case 0x4004:
        pulse2.writeControl(data);
        break;
    case 0x4006:
        pulse2.writeTimerLo(data);
        break;
    case 0x4007:
        pulse2.writeTimerHi(data);
        break;
    case 0x4015:
        status.reg = data;
        break;
    default:
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
    pulse2.stepTimer();
}

void APU::stepFrameCounter() {
    static std::uint16_t i = 0;

    if ((i % FrameCounterPeriod) == 0) {
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
            // assert(0);
            break;
        }

        if (++frameCounter == frameCounterMode) {
            frameCounter = 0;
        }
    }

    if (++i == FrameCounterPeriod) {
        i = 0;
    }
}

void APU::sendSample() {
    assert(sampleRate > 0);
    assert(ApuFrequency > sampleRate);

    static std::uint32_t i = 0;

    std::uint32_t period = ApuFrequency / sampleRate;

    if ((i % period) == 0) {
        sampleCallback(getOutputSample());
    }

    if (++i == period) {
        i = 0;
    }
}

double APU::getOutputSample() {
    std::uint8_t pulse1Out = status.pulse1Enabled() ? pulse1.output() : 0;
    std::uint8_t pulse2Out = status.pulse2Enabled() ? pulse2.output() : 0;
    return pulse1Out + pulse2Out;
}
