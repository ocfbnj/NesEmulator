#ifndef OCFBNJ_NES_TRIANGLE_H
#define OCFBNJ_NES_TRIANGLE_H

#include <cstdint>

#include <nes/APU/LengthCounter.h>
#include <nes/APU/Timer.h>

class Triangle {
public:
    bool lengthGreaterThanZero() const;
    std::uint8_t output() const;

    void resetLengthCounter();

    void stepTimer();
    void stepLengthCounter();
    void stepLinearCounter();

    void writeControl(std::uint8_t data);
    void writeTimerLo(std::uint8_t data);
    void writeTimerHi(std::uint8_t data);

private:
    std::uint8_t linearCounterPeriod = 0;
    std::uint8_t linearCounterValue = 0;
    std::uint8_t dutyValue = 0;
    bool reload = false;
    bool control = false;

    Timer timer;
    LengthCounter lengthCounter;
};

#endif // OCFBNJ_NES_TRIANGLE_H
