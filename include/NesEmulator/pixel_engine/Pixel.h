#ifndef NESEMULATOR_PIXEL_H
#define NESEMULATOR_PIXEL_H

#include <array>
#include <cstdint>

struct Pixel {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
};

bool operator==(const Pixel& l, const Pixel& r);

extern std::array<Pixel, 64> SystemPalette;

#endif // NESEMULATOR_PIXEL_H
