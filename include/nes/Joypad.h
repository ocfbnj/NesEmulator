#ifndef OCFBNJ_NES_JOYPAD_H
#define OCFBNJ_NES_JOYPAD_H

#include <cstdint>

class Joypad {
public:
    enum class Button : std::uint8_t {
        Right = 0b10000000,
        Left = 0b01000000,
        Down = 0b00100000,
        Up = 0b00010000,
        Start = 0b00001000,
        Select = 0b00000100,
        B = 0b00000010,
        A = 0b00000001,
    };

    std::uint8_t read();
    void write(std::uint8_t data);

    void press(Button btn);
    void release(Button btn);

private:
    std::uint8_t button = 0;
    std::uint8_t shifter = 0;
};

#endif // OCFBNJ_NES_JOYPAD_H
