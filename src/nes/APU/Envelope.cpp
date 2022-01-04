#include <nes/APU/Envelope.h>

std::uint8_t Envelope::getVolume() const {
    return c ? v : decayLevel;
}

void Envelope::write(std::uint8_t value) {
    data = value;
}

void Envelope::setStartFlag() {
    start = true;
}

void Envelope::step() {
    if (start) {
        start = false;
        decayLevel = 15;
        counter = v;
    } else {
        if (counter == 0) {
            counter = v;
            if (decayLevel == 0 && l) {
                decayLevel = 15;
            } else if (decayLevel > 0) {
                decayLevel--;
            }
        } else if (counter > 0) {
            counter--;
        }
    }
}
