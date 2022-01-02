#include <array>
#include <cassert>
#include <cmath>

#include <nes/APU.h>
#include <nes/literals.h>

void APU::clock() {
    stepTimer();

    stepFrameCounter();
    sendSample();
}

void APU::reset() {
}

std::uint8_t APU::apuRead(std::uint16_t addr) {
    assert(addr == 0x4015);

    std::uint8_t data = 0;
    if (addr == 0x4015) {
        data = readStatus();
    }

    return data;
}

void APU::apuWrite(std::uint16_t addr, std::uint8_t data) {
    assert(addr >= 0x4000 && addr < 0x4009 || addr >= 0x400A && addr < 0x400D || addr >= 0x400E && addr < 0x4014 || addr == 0x4015 || addr == 0x4017);
    switch (addr) {
    case 0x4000:
        pulse1.writeControl(data);
        break;
    case 0x4001:
        pulse1.writeSweep(data);
        break;
    case 0x4002:
        pulse1.writeTimerLo(data);
        break;
    case 0x4003:
        pulse1.writeTimerHi(data);
        break;
    case 0x4004:
        pulse2.writeControl(data);
        break;
    case 0x4005:
        pulse2.writeSweep(data);
        break;
    case 0x4006:
        pulse2.writeTimerLo(data);
        break;
    case 0x4007:
        pulse2.writeTimerHi(data);
        break;
    case 0x4015:
        writeStatus(data);
        break;
    case 0x4017:
        writeFrameCounter(data);
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

std::uint8_t APU::readStatus() const {
    std::uint8_t data = 0;

    if (pulse1.lengthCounter.value > 0) {
        data |= 0b0000'0001;
    }

    if (pulse2.lengthCounter.value > 0) {
        data |= 0b0000'0010;
    }

    return data;
}

void APU::writeStatus(std::uint8_t data) {
    status.reg = data;

    if (!status.pulse1Enabled()) {
        pulse1.lengthCounter.value = 0;
    }

    if (!status.pulse2Enabled()) {
        pulse2.lengthCounter.value = 0;
    }
}

void APU::writeFrameCounter(std::uint8_t data) {
    frameCounterMode = 4 + ((data >> 7) & 1);
    irqInhibit = (data >> 6) & 1;
}

void APU::stepTimer() {
    pulse1.stepTimer();
    pulse2.stepTimer();
}

void APU::stepLengthCounter() {
    pulse1.stepLengthCounter();
    pulse2.stepLengthCounter();
}

void APU::stepEnvelope() {
    pulse1.stepEnvelope();
    pulse2.stepEnvelope();
}

void APU::stepSweep() {
    pulse1.stepSweep();
    pulse2.stepSweep();
}

void APU::stepFrameCounter() {
    static std::uint64_t i = 0;

    if ((i % FrameCounterPeriod) == 0) {
        switch (frameCounterMode) {
        case 4:
            switch (frameCounter) {
            case 0:
            case 2:
                stepEnvelope();
                break;
            case 1:
                stepLengthCounter();
                stepEnvelope();
                stepSweep();
                break;
            case 3:
                stepLengthCounter();
                stepEnvelope();
                stepSweep();
                // TODO irq
                break;
            default:
                assert(0);
                break;
            }
            break;
        case 5:
            switch (frameCounter) {
            case 0:
            case 2:
                stepLengthCounter();
                stepEnvelope();
                stepSweep();
                break;
            case 1:
            case 3:
                stepEnvelope();
                break;
            case 4:
                // do nothing
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

        frameCounter = (frameCounter + 1) % frameCounterMode;
    }

    i = (i + 1) % FrameCounterPeriod;
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
