#include <array>

#include <nes/APU/DMC.h>
#include <nes/Bus.h>

namespace {
constexpr std::array<std::uint16_t, 16> DmcTable{214, 190, 170, 160, 143, 127, 113, 107, 95, 80, 71, 64, 53, 42, 36, 27};
}

std::uint16_t DMC::getCurrentLength() const {
    return currentLength;
}

std::uint8_t DMC::output() const {
    return value;
}

void DMC::connect(Bus* bus) {
    this->bus = bus;
}

void DMC::resetCurrentLength() {
    currentLength = 0;
}

void DMC::stepTimer() {
    stepReader();

    if (timer.step()) {
        stepShifter();
    }
}

void DMC::stepReader() {
    if (currentLength > 0 && bitCount == 0) {
        assert(bus != nullptr);
        shiftRegister = bus->cpuRead(currentAddress);

        bitCount = 8;

        if (++currentAddress == 0) {
            currentAddress = 0x8000;
        }

        if (--currentLength == 0 && l) {
            restart();
        }
    }
}

void DMC::stepShifter() {
    if (bitCount == 0) {
        return;
    }

    if (shiftRegister & 1) {
        if (value <= 125) {
            value += 2;
        }
    } else {
        if (value >= 2) {
            value -= 2;
        }
    }

    shiftRegister >>= 1;
    bitCount--;
}

void DMC::restart() {
    currentAddress = sampleAddress;
    currentLength = sampleLength;
}

void DMC::writeControl(std::uint8_t data) {
    flagsAndRate = data;
    timer.period = DmcTable[r];
}

void DMC::writeValue(std::uint8_t data) {
    value = data & 0x7F;
}

void DMC::writeAddress(std::uint8_t data) {
    // Sample address = %11AAAAAA.AA000000 = $C000 + (A * 64)
    sampleAddress = 0xC000 + data * 64;
}

void DMC::writeLength(std::uint8_t data) {
    // Sample length = %LLLL.LLLL0001 = (L * 16) + 1 bytes
    sampleLength = (data * 16) + 1;
}
