#ifndef OCFBNJ_NES_DMC_H
#define OCFBNJ_NES_DMC_H

#include <cstdint>

#include <nes/APU/Timer.h>

class Bus;

class DMC {
public:
    std::uint16_t getCurrentLength() const;
    std::uint8_t output() const;

    void connect(Bus* bus);

    void resetCurrentLength();

    void stepTimer();
    void stepReader();
    void stepShifter();

    void restart();

    void writeControl(std::uint8_t data);
    void writeValue(std::uint8_t data);
    void writeAddress(std::uint8_t data);
    void writeLength(std::uint8_t data);

private:
    union {
        struct {
            std::uint8_t r : 4; // Rate index
            std::uint8_t unused : 2;
            std::uint8_t l : 1; // Loop flag
            std::uint8_t i : 1; // IRQ enabled flag. If clear, the interrupt flag is cleared.
        };

        std::uint8_t flagsAndRate = 0;
    };

    std::uint8_t value = 0;
    std::uint16_t sampleAddress = 0;
    std::uint16_t sampleLength = 0;
    std::uint16_t currentAddress = 0;
    std::uint16_t currentLength = 0;
    std::uint8_t bitCount = 0;
    std::uint8_t shiftRegister = 0;

    Timer timer;

    Bus* bus = nullptr;
};

#endif // OCFBNJ_NES_DMC_H
