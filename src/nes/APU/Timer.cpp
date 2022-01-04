#include <nes/APU/Timer.h>

bool Timer::step() {
    if (counter == 0) {
        counter = period;
        return true;
    } else if (counter > 0) {
        counter--;
    }

    return false;
}
