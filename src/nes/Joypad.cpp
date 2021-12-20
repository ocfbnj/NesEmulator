#include "Joypad.h"

void Joypad::press(Button btn) {
    button |= static_cast<uint8_t>(btn);
}

void Joypad::release(Button btn) {
    button &= ~(static_cast<uint8_t>(btn));
}

uint8_t Joypad::read() {
    uint8_t bit = shifter & 1;
    shifter >>= 1;

    return bit;
}

void Joypad::write(uint8_t data) {
    if (data & 1) {
        shifter = button;
    }
}
