#ifndef OCFBNJ_NES_SWEEP_H
#define OCFBNJ_NES_SWEEP_H

#include <cstdint>

class Sweep {
public:
    void write(std::uint8_t value);
    void setReloadFlag();

    void step(std::uint16_t& timerPeriod, bool muting);

private:
    void sweep(std::uint16_t& timerPeriod);

    union {
        struct {
            std::uint8_t s : 3; // Shift count (number of bits)
            std::uint8_t n : 1; // Negate flag; 0: add to period, sweeping toward lower frequencies; 1: subtract from period, sweeping toward higher frequencies
            std::uint8_t p : 3; // The divider's period is P + 1 half-frames
            std::uint8_t e : 1; // Enabled flag
        };

        std::uint8_t data = 0;
    };

    bool reload = false;
    std::uint8_t counter = 0;
};

#endif // OCFBNJ_NES_SWEEP_H
