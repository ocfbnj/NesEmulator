#include <nes/APU/Sweep.h>

void Sweep::write(std::uint8_t value) {
    data = value;
}

void Sweep::setReloadFlag() {
    reload = true;
}

void Sweep::step(std::uint16_t& timerPeriod, bool muting) {
    if (counter == 0 && e && !muting) {
        // The pulse's period is adjusted.
        sweep(timerPeriod);
    }

    if (counter == 0 || reload) {
        counter = p + 1;
        if (reload) {
            reload = false;
        }
    } else if (counter > 0) {
        counter--;
    }
}

void Sweep::sweep(std::uint16_t& timerPeriod) {
    std::uint16_t delta = timerPeriod >> s;
    if (n) {
        timerPeriod -= delta;
    } else {
        timerPeriod += delta;
    }
}
