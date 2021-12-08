#ifndef NESEMULATOR_JOYPAD_H
#define NESEMULATOR_JOYPAD_H

#include <cstdint>

class Joypad {
public:
    void write(uint8_t data) {
        strobe = data & 1;
        if (strobe) {
            index = 0;
        }
    }

    [[nodiscard]] uint8_t read() {
        if (index > 7) {
            return 1;
        }

        uint8_t res = (button & (1 << index)) >> index;
        if (!strobe && index <= 7) {
            index++;
        }

        return res;
    }

private:
    bool strobe = false;
    int index = 0;
    uint8_t button = 0;
};

#endif // NESEMULATOR_JOYPAD_H
