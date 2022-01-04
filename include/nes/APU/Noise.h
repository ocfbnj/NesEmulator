#ifndef OCFBNJ_NES_NOISE_H
#define OCFBNJ_NES_NOISE_H

#include <cstdint>

#include <nes/APU/Envelope.h>
#include <nes/APU/LengthCounter.h>
#include <nes/APU/Timer.h>

class Noise {
public:
    bool lengthGreaterThanZero() const;
    std::uint8_t output() const;

    void resetLengthCounter();

    void stepTimer();
    void stepLengthCounter();
    void stepEnvelope();

    void writeControl(std::uint8_t data);
    void writePeriod(std::uint8_t data);
    void writeLengthCounter(std::uint8_t data);

private:
    std::uint8_t mode = 0;
    std::uint16_t shiftRegister = 0x7F;

    Timer timer;
    LengthCounter lengthCounter;
    Envelope envelope;
};

#endif // OCFBNJ_NES_NOISE_H
