#ifndef OCFBNJ_NES_LITERALS_H
#define OCFBNJ_NES_LITERALS_H

#include <array>

constexpr auto CpuFrequency = 1789773;
constexpr auto ApuFrequency = CpuFrequency / 2;
constexpr auto PpuFrequency = CpuFrequency * 3;

// CPU clock frequency is 1.789773 MHz
// PPU clock frequency is three times CPU (~5.369319 MHz)
// A frame has 341 x 262 = 89,342 clock cycles
// So NES can output 5,369,319 / 89,342 ~= 60.098 frame per seconds
constexpr auto FPS = 60;

// The sampling rate is not the usual 44100,
// because my APU actually produces a sampling rate of 44671,
// and setting it to 44100 does not work well.
constexpr auto SampleRate = 44671;
constexpr auto SampleCountPerFrame = SampleRate / FPS;

constexpr auto FrameCounterFrequency = 240;
constexpr auto FrameCounterPeriod = ApuFrequency / FrameCounterFrequency;

constexpr std::array<std::uint8_t, 4> DutyCycleSequences{
    0b0100'0000, // 12.5%
    0b0110'0000, // 25%
    0b0111'1000, // 50%
    0b1001'1111, // 25% negated
};

constexpr std::array<std::uint8_t, 32> LengthTable{10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};
constexpr std::array<std::uint16_t, 16> NoiseTable{4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};

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

constexpr auto operator"" _kb(unsigned long long byte) {
    return 1024 * byte;
}

#endif // OCFBNJ_NES_LITERALS_H
