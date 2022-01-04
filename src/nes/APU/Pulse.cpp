#include <nes/APU/Pulse.h>

#include <array>

namespace {
constexpr std::array<std::uint8_t, 4> DutyCycleSequences{
    0b0100'0000, // 12.5%
    0b0110'0000, // 25%
    0b0111'1000, // 50%
    0b1001'1111, // 25% negated
};
}

bool Pulse::lengthGreaterThanZero() const {
    return lengthCounter.greaterZero();
}

std::uint8_t Pulse::output() const {
    if (muting()) {
        return 0;
    }

    if (((DutyCycleSequences[dutyCycle] >> (7 - dutyValue)) & 1) == 0) {
        return 0;
    }

    if (lengthCounter.isZero()) {
        return 0;
    }

    return envelope.getVolume();
}

void Pulse::resetLengthCounter() {
    lengthCounter.setCounter(0);
}

void Pulse::stepTimer() {
    if (timer.step()) {
        dutyValue = (dutyValue + 1) % 8;
    }
}

void Pulse::stepLengthCounter() {
    lengthCounter.step();
}

void Pulse::stepEnvelope() {
    envelope.step();
}

void Pulse::stepSweep() {
    sweep.step(timer.period, muting());
}

void Pulse::writeControl(std::uint8_t data) {
    dutyCycle = (data >> 6) & 0b11;

    lengthCounter.setHalt((data >> 5) & 1);

    envelope.write(data);
    envelope.setStartFlag();
}

void Pulse::writeSweep(std::uint8_t data) {
    sweep.write(data);
    sweep.setReloadFlag();
}

void Pulse::writeTimerLo(std::uint8_t data) {
    timer.period = (timer.period & 0xFF00) | static_cast<std::uint16_t>(data);
}

void Pulse::writeTimerHi(std::uint8_t data) {
    timer.period = (timer.period & 0x00FF) | ((static_cast<std::uint16_t>(data) & 0b111) << 8);
    lengthCounter.setCounter(LengthTable[data >> 3]);
    envelope.setStartFlag();

    dutyValue = 0;
}

bool Pulse::muting() const {
    return timer.period < 8 || timer.period > 0x7FF;
}
