#include <nes/APU/Noise.h>

#include <array>

namespace {
constexpr std::array<std::uint16_t, 16> NoiseTable{4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};
}

bool Noise::lengthGreaterThanZero() const {
    return lengthCounter.greaterZero();
}

std::uint8_t Noise::output() const {
    // The mixer receives the current envelope volume except when
    // - Bit 0 of the shift register is set, or
    // - The length counter is zero
    if (shiftRegister & 1) {
        return 0;
    }

    if (lengthCounter.isZero()) {
        return 0;
    }

    return envelope.getVolume();
}

void Noise::resetLengthCounter() {
    lengthCounter.setCounter(0);
}

void Noise::stepTimer() {
    if (timer.step()) {
        // When the timer clocks the shift register, the following actions occur in order:
        // 1. Feedback is calculated as the exclusive-OR of bit 0 and one other bit: bit 6 if Mode flag is set, otherwise bit 1.
        // 2. The shift register is shifted right by one bit.
        // 3. Bit 14, the leftmost bit, is set to the feedback calculated earlier.
        std::uint8_t shift = mode ? 6 : 1;

        std::uint16_t b1 = shiftRegister & 1;
        std::uint16_t b2 = (shiftRegister >> shift) & 1;

        shiftRegister >>= 1;
        shiftRegister |= (b1 ^ b2) << 14;
    }
}

void Noise::stepLengthCounter() {
    lengthCounter.step();
}

void Noise::stepEnvelope() {
    envelope.step();
}

void Noise::writeControl(std::uint8_t data) {
    lengthCounter.setHalt((data >> 5) & 1);

    envelope.write(data);
    envelope.setStartFlag();
}

void Noise::writePeriod(std::uint8_t data) {
    mode = (data >> 7) & 1;
    timer.period = NoiseTable[data & 0x0F];
}

void Noise::writeLengthCounter(std::uint8_t data) {
    lengthCounter.setCounter(LengthTable[data >> 3]);
    envelope.setStartFlag();
}
