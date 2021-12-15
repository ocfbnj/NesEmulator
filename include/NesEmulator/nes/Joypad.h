#ifndef OCFBNJ_NES_JOYPAD_H
#define OCFBNJ_NES_JOYPAD_H

#include <cstdint>

class Joypad {
public:
    enum class Button : uint8_t {
        Right = 0b10000000,
        Left = 0b01000000,
        Down = 0b00100000,
        Up = 0b00010000,
        Start = 0b00001000,
        Select = 0b00000100,
        B = 0b00000010,
        A = 0b00000001,
    };

    void press(Button btn);
    void release(Button btn);

    void write(uint8_t data);
    [[nodiscard]] uint8_t read() const;

private:
    uint8_t button = 0;
    mutable uint8_t shifter = 0;
};

#endif // NESEMULATOR_JOYPAD_H
