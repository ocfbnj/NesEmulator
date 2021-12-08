#include "Joypad.h"

void Joypad::press(Joypad::Button btn) {
    button |= uint8_t(btn);
}

void Joypad::release(Button btn) {
    button &= ~(uint8_t(btn));
}

void Joypad::write(uint8_t data) {
    strobe = data & 1;
    if (strobe) {
        index = 0;
    }
}

uint8_t Joypad::read() {
    if (index > 7) {
        return 1;
    }

    uint8_t res = (button & (1 << index)) >> index;
    if (!strobe && index <= 7) {
        index++;
    }

    return res;
}
