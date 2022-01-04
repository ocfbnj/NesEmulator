#ifndef OCFBNJ_NES_PULSE_H
#define OCFBNJ_NES_PULSE_H

#include <cstdint>

#include <nes/APU/Envelope.h>
#include <nes/APU/LengthCounter.h>
#include <nes/APU/Sweep.h>
#include <nes/APU/Timer.h>

class Pulse {
public:
    bool lengthGreaterThanZero() const;
    std::uint8_t output() const;

    void resetLengthCounter();

    void stepTimer();
    void stepLengthCounter();
    void stepEnvelope();
    void stepSweep();

    void writeControl(std::uint8_t data);
    void writeSweep(std::uint8_t data);
    void writeTimerLo(std::uint8_t data);
    void writeTimerHi(std::uint8_t data);

private:
    bool muting() const;

    std::uint8_t dutyCycle = 0;
    std::uint8_t dutyValue = 0;

    Timer timer;
    LengthCounter lengthCounter;
    Envelope envelope;
    Sweep sweep;
};

#endif // OCFBNJ_NES_PULSE_H
