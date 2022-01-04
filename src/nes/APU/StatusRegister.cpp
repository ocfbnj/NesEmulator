#include <nes/APU/StatusRegister.h>

bool StatusRegister::pulse1Enabled() const {
    return p1;
}

bool StatusRegister::pulse2Enabled() const {
    return p2;
}

bool StatusRegister::triangleEnabled() const {
    return t;
}

bool StatusRegister::noiseEnabled() const {
    return n;
}

bool StatusRegister::dmcEnabled() const {
    return d;
}

void StatusRegister::write(std::uint8_t data) {
    reg = data;
}
