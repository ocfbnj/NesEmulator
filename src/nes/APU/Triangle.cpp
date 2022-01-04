#include <nes/APU/Triangle.h>

#include <array>

namespace {
constexpr std::array<std::uint8_t, 32> TriangleTable{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
}

bool Triangle::lengthGreaterThanZero() const {
    return lengthCounter.greaterZero();
}

std::uint8_t Triangle::output() const {
    if (linearCounterValue == 0) {
        return 0;
    }

    if (lengthCounter.isZero()) {
        return 0;
    }

    return TriangleTable[dutyValue];
}

void Triangle::resetLengthCounter() {
    lengthCounter.setCounter(0);
}

void Triangle::stepTimer() {
    if (timer.step()) {
        if (linearCounterValue > 0 && lengthCounter.greaterZero()) {
            dutyValue = (dutyValue + 1) % 32;
        }
    }
}

void Triangle::stepLengthCounter() {
    lengthCounter.step();
}

void Triangle::stepLinearCounter() {
    if (reload) {
        linearCounterValue = linearCounterPeriod;
    } else if (linearCounterValue > 0) {
        linearCounterValue--;
    }

    if (!control) {
        reload = false;
    }
}

void Triangle::writeControl(std::uint8_t data) {
    lengthCounter.setHalt((data >> 7) & 1);

    control = (data >> 7) & 1;
    linearCounterPeriod = data & 0x7F;
}

void Triangle::writeTimerLo(std::uint8_t data) {
    timer.period = (timer.period & 0xFF00) | static_cast<std::uint16_t>(data);
}

void Triangle::writeTimerHi(std::uint8_t data) {
    Triangle::timer.period = (timer.period & 0x00FF) | ((static_cast<std::uint16_t>(data) & 0b111) << 8);
    lengthCounter.setCounter(LengthTable[data >> 3]);

    reload = true;
}
