#include <array>
#include <cassert>
#include <cmath>

#include <nes/APU.h>
#include <nes/Bus.h>
#include <nes/literals.h>

namespace {
static const std::array<double, 31> PulseTable = [] {
    std::array<double, 31> res;

    for (int i = 0; i != res.size(); i++) {
        res[i] = 95.52 / (8128.0 / i + 100);
    }

    return res;
}();

static const std::array<double, 203> TndTable = [] {
    std::array<double, 203> res;

    for (int i = 0; i != res.size(); i++) {
        res[i] = 163.67 / (24329.0 / i + 100);
    }

    return res;
}();
} // namespace

void APU::connect(Bus* bus) {
    this->bus = bus;
    dmc.connect(bus);
}

void APU::clock() {
    stepTimer();

    // i:      0 1 2 3 4 5 6 7 8 9
    //         - - - - - - - - - -
    // period: 3 3 3 3 3 3 3 3 3 3
    // A, B:   0 0 0 1 1 1 2 2 2 3
    //          x x o x x o x x o

    std::uint64_t a = i;
    i++;
    std::uint64_t b = i;

    double frameCounterPeriod = static_cast<double>(ApuFrequency) / FrameCounterFrequency;
    auto frameCounterA = static_cast<std::uint64_t>(a / frameCounterPeriod);
    auto frameCounterB = static_cast<std::uint64_t>(b / frameCounterPeriod);
    if (frameCounterA != frameCounterB) {
        stepFrameCounter();
    }

    double samplePeriod = static_cast<double>(ApuFrequency) / sampleRate;
    auto sampleA = static_cast<std::uint64_t>(a / samplePeriod);
    auto sampleB = static_cast<std::uint64_t>(b / samplePeriod);
    if (sampleA != sampleB) {
        sendSample();
    }
}

void APU::reset() {
    frameCounter = 0;
    frameCounterMode = 4;
    irqInhibit = false;
    i = 0;
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
    case 0x4008:
        triangle.writeControl(data);
        break;
    case 0x400A:
        triangle.writeTimerLo(data);
        break;
    case 0x400B:
        triangle.writeTimerHi(data);
        break;
    case 0x400C:
        noise.writeControl(data);
        break;
    case 0x400E:
        noise.writePeriod(data);
        break;
    case 0x400F:
        noise.writeLengthCounter(data);
        break;
    case 0x4010:
        dmc.writeControl(data);
        break;
    case 0x4011:
        dmc.writeValue(data);
        break;
    case 0x4012:
        dmc.writeAddress(data);
        break;
    case 0x4013:
        dmc.writeLength(data);
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

    assert(sampleRate > 0);
    assert(ApuFrequency > sampleRate);
}

void APU::setSampleCallback(SampleCallback callback) {
    sampleCallback = std::move(callback);
}

void APU::serialize(std::ostream& os) const {
    auto begin = reinterpret_cast<const char*>(this) + offsetof(APU, frameCounter);
    auto end = reinterpret_cast<const char*>(this) + offsetof(APU, bus);
    os.write(begin, end - begin);
}

void APU::deserialize(std::istream& is) {
    auto begin = reinterpret_cast<char*>(this) + offsetof(APU, frameCounter);
    auto end = reinterpret_cast<char*>(this) + offsetof(APU, bus);
    is.read(begin, end - begin);

    dmc.connect(bus);
}

std::uint8_t APU::readStatus() const {
    std::uint8_t data = 0;

    if (pulse1.lengthGreaterThanZero()) {
        data |= 0b0000'0001;
    }

    if (pulse2.lengthGreaterThanZero()) {
        data |= 0b0000'0010;
    }

    if (triangle.lengthGreaterThanZero()) {
        data |= 0b0000'0100;
    }

    if (noise.lengthGreaterThanZero()) {
        data |= 0b0000'1000;
    }

    if (dmc.getCurrentLength() > 0) {
        data |= 0b0001'0000;
    }

    return data;
}

void APU::writeStatus(std::uint8_t data) {
    status.write(data);

    if (!status.pulse1Enabled()) {
        pulse1.resetLengthCounter();
    }

    if (!status.pulse2Enabled()) {
        pulse2.resetLengthCounter();
    }

    if (!status.triangleEnabled()) {
        triangle.resetLengthCounter();
    }

    if (!status.noiseEnabled()) {
        noise.resetLengthCounter();
    }

    if (!status.dmcEnabled()) {
        dmc.resetCurrentLength();
    } else {
        if (dmc.getCurrentLength() == 0) {
            dmc.restart();
        }
    }
}

void APU::writeFrameCounter(std::uint8_t data) {
    frameCounterMode = 4 + ((data >> 7) & 1);
    irqInhibit = (data >> 6) & 1;

    frameCounter = 0;
    if (frameCounterMode == 5) {
        stepLengthCounter();
        stepEnvelopeAndLinearCounter();
        stepSweep();
    }
}

void APU::stepTimer() {
    pulse1.stepTimer();
    pulse2.stepTimer();
    triangle.stepTimer();
    triangle.stepTimer();
    noise.stepTimer();

    if (status.dmcEnabled()) {
        dmc.stepTimer();
    }
}

void APU::stepLengthCounter() {
    pulse1.stepLengthCounter();
    pulse2.stepLengthCounter();
    triangle.stepLengthCounter();
    noise.stepLengthCounter();
}

void APU::stepEnvelopeAndLinearCounter() {
    pulse1.stepEnvelope();
    pulse2.stepEnvelope();
    triangle.stepLinearCounter();
    noise.stepEnvelope();
}

void APU::stepSweep() {
    pulse1.stepSweep();
    pulse2.stepSweep();
}

void APU::stepFrameCounter() {
    // mode 0:    mode 1:       function
    // ---------  -----------  -----------------------------
    //  - - - f    - - - - -    IRQ (if bit 6 is clear)
    //  - l - l    - l - - l    Length counter and sweep
    //  e e e e    e e e - e    Envelope and linear counter

    switch (frameCounterMode) {
    case 4:
        switch (frameCounter) {
        case 0:
        case 2:
            stepEnvelopeAndLinearCounter();
            break;
        case 1:
            stepLengthCounter();
            stepSweep();
            stepEnvelopeAndLinearCounter();
            break;
        case 3:
            stepLengthCounter();
            stepSweep();
            stepEnvelopeAndLinearCounter();
            if (!irqInhibit) {
                assert(bus != nullptr);
                bus->getCPU().irq();
            }
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
            stepEnvelopeAndLinearCounter();
            break;
        case 1:
        case 4:
            stepLengthCounter();
            stepSweep();
            stepEnvelopeAndLinearCounter();
            break;
        case 3:
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

void APU::sendSample() {
    if (sampleCallback) {
        sampleCallback(getOutputSample());
    }
}

double APU::getOutputSample() const {
    std::uint8_t pulse1Out = status.pulse1Enabled() ? pulse1.output() : 0;
    std::uint8_t pulse2Out = status.pulse2Enabled() ? pulse2.output() : 0;

    std::uint8_t triangleOut = status.triangleEnabled() ? triangle.output() : 0;
    std::uint8_t noiseOut = status.noiseEnabled() ? noise.output() : 0;
    std::uint8_t dmcOut = status.dmcEnabled() ? dmc.output() : 0;

    double pulseOut = PulseTable[pulse1Out + pulse2Out];
    double tndOut = TndTable[3 * triangleOut + 2 * noiseOut + dmcOut];

    return pulseOut + tndOut;
}
