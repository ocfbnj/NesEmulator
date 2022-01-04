#include <nes/APU/LengthCounter.h>

bool LengthCounter::isZero() const {
    return counter == 0;
}

bool LengthCounter::greaterZero() const {
    return counter > 0;
}

void LengthCounter::setCounter(std::uint8_t value) {
    counter = value;
}

void LengthCounter::setHalt(bool b) {
    halt = b;
}

void LengthCounter::step() {
    if (counter > 0 && !halt) {
        counter--;
    }
}
