#ifndef OCFBNJ_NES_LITERALS_H
#define OCFBNJ_NES_LITERALS_H

// CPU clock frequency is 1.789773 MHz
// PPU clock frequency is three times CPU (~5.369319 MHz)
// A frame has 341 x 262 = 89,342 clock cycles
// So NES can output 5,369,319 / 89,342 ~= 60.098 frame per seconds
constexpr auto FPS = 60;

constexpr auto PpuFrequency = FPS * 89'342;
constexpr auto CpuFrequency = PpuFrequency / 3;
constexpr auto ApuFrequency = CpuFrequency / 2;

constexpr auto SampleRate = 44100;
constexpr auto SampleCountPerFrame = SampleRate / FPS;

constexpr auto FrameCounterFrequency = 240;

constexpr auto operator"" _kb(unsigned long long byte) {
    return 1024 * byte;
}

#endif // OCFBNJ_NES_LITERALS_H
