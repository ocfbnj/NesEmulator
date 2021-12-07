#ifndef NESEMULATOR_PIXEL_H
#define NESEMULATOR_PIXEL_H

#include <cstdint>
#include <array>

struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

extern std::array<Pixel, 64> SystemPalette;

#endif // NESEMULATOR_PIXEL_H
