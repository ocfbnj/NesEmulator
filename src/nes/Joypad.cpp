#include "Joypad.h"

void Joypad::press(Joypad::Button btn) {
    button |= uint8_t(btn);
}

void Joypad::release(Button btn) {
    button &= ~(uint8_t(btn));
}

void Joypad::write(uint8_t data) {
    if (data & 1) {
        shifter = button;
    }
}

uint8_t Joypad::read() const {
    uint8_t bit = shifter & 1;
    shifter >>= 1;

    return bit;
}
