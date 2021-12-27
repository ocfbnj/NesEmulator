#include <nes/Joypad.h>

std::uint8_t Joypad::read() {
    std::uint8_t bit = shifter & 1;
    shifter >>= 1;

    return bit;
}

void Joypad::write(std::uint8_t data) {
    if (data & 1) {
        shifter = button;
    }
}

void Joypad::press(Button btn) {
    button |= static_cast<std::uint8_t>(btn);
}

void Joypad::release(Button btn) {
    button &= ~(static_cast<std::uint8_t>(btn));
}
