#ifndef OCFBNJ_NES_LENGTH_COUNTER_H
#define OCFBNJ_NES_LENGTH_COUNTER_H

#include <array>
#include <cstdint>

constexpr std::array<std::uint8_t, 32> LengthTable{10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};

class LengthCounter {
public:
    bool isZero() const;
    bool greaterZero() const;

    void setCounter(std::uint8_t value);
    void setHalt(bool b);

    void step();

private:
    std::uint8_t counter = 0;
    bool halt = false;
};

#endif // OCFBNJ_NES_LENGTH_COUNTER_H
